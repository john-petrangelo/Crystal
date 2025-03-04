#include <cstring>
#include <format>
#include <iomanip>

#include <lwip/prot/dhcp.h>
#include <lwip/ip4_addr.h>
#include <lwip/udp.h>

#include <lumos-arduino/Logger.h>

#include <Networking/NetworkUtils.h>

#include "DHCPServer.h"


void DHCPServer::parseDHCPOptions(uint8_t const *payload, uint16_t const len) {
    uint16_t constexpr DHCP_OPTIONS_START = 240;

    // Check minimum packet length
    if (len < DHCP_OPTIONS_START) {
        logger << "DHCP packet too short, skipping options parsing" << std::endl;
        return;
    }

    uint16_t options_offset = DHCP_OPTIONS_START;

    while (options_offset < len) {
        uint8_t const option = payload[options_offset];
        if (option == DHCP_OPTION_END) { // End of options
            break;
        }
        if (option == DHCP_OPTION_PAD) {
            // Skip the pad option
            options_offset++;
            continue;
        }
        if (options_offset + 1 >= len) {
            // Option is out-of-bounds of payload, break to avoid buffer overflow
            logger << "DHCP option out-of-bounds (offset: " << options_offset
                   << ", len: " << len << "), skipping" << std::endl;
            break;
        }

        uint8_t const option_len = payload[options_offset + 1];
        if (options_offset + 2 + option_len > len) {
            // Option is out-of-bounds of payload, break to avoid buffer overflow
            logger << "DHCP option " << static_cast<int>(option) << "(len: " << option_len
                   << ") exceeds packet bounds (offset: " << options_offset
                   << ", len: " << len << "), skipping" << std::endl;
            break;
        }

        uint8_t const *option_data = payload + options_offset + 2;

        switch (option) {
            case DHCP_OPTION_REQUESTED_IP:
                if (option_len == sizeof(options.requestedIP)) {
                    std::memcpy(&options.requestedIP, option_data, sizeof(options.requestedIP));
                    options.requestedIP = ntohl(options.requestedIP);
                }
            break;
            case DHCP_OPTION_MESSAGE_TYPE:
                if (option_len == DHCP_OPTION_MESSAGE_TYPE_LEN) {
                    options.messageType = *option_data;
                }
                break;
            case DHCP_OPTION_PARAMETER_REQUEST_LIST:
                if (option_len > 0) {
                    options.parameterRequestList.assign(option_data, option_data + option_len);
                }
                break;
            case DHCP_OPTION_MAX_MSG_SIZE:
                if (option_len == DHCP_OPTION_MAX_MSG_SIZE_LEN) {
                    std::memcpy(&options.maxMessageSize, option_data, sizeof(options.maxMessageSize));
                    options.maxMessageSize = ntohs(options.maxMessageSize);
                }
                break;
            case DHCP_OPTION_LEASE_TIME:
            case DHCP_OPTION_SERVER_ID:
            case DHCP_OPTION_CLIENT_ID:
                // Silently ignoring these options
                break;
            default:
                logger << "DHCP encountered unrecognized option: " << static_cast<int>(option) << std::endl;
                break;
        }

        // Move on to the next option
        options_offset += 2 + option_len;
    }
}

void DHCPServer::appendDHCPOption(uint8_t const code, uint8_t const length, uint8_t *payload, uint16_t &offset, const uint8_t *data) {
    payload[offset++] = code;
    payload[offset++] = length;
    memcpy(&payload[offset], data, length);
    offset += length;
}

void DHCPServer::appendDHCPMsgTypeOption(uint8_t *payload, uint16_t &offset, uint8_t msgType) {
    payload[offset++] = DHCP_OPTION_MESSAGE_TYPE;
    payload[offset++] = DHCP_OPTION_MESSAGE_TYPE_LEN;
    payload[offset++] = msgType;
}

bool DHCPServer::handleDHCPDiscover(struct udp_pcb *pcb, const ip_addr_t *addr, uint32_t const xid, uint8_t mac[6]) {
    logger << "DHCP type DISCOVER" << std::endl;
    auto const ip = leasePool.allocate_ip(mac);
    if (!ip) {
        logger << "No IP available for " << macAddrToString(mac) << std::endl << std::endl;
        return true;
    }
    auto const ipAddr = ip.value().addr;

    // Create DHCP OFFER response
    struct pbuf *response = pbuf_alloc(PBUF_TRANSPORT, 300, PBUF_RAM);
    auto const resp_payload = static_cast<uint8_t *>(response->payload);
    memset(resp_payload, 0, 300);

    resp_payload[0] = 2; // Message type: Boot Reply

    resp_payload[1] = 0x01; // Hardware type: Ethernet
    resp_payload[2] = 0x06; // Hardware address length: 6

    *(uint32_t *)&resp_payload[4] = htonl(xid); // Transaction ID

    resp_payload[10] = 0x00; // Unicast
    resp_payload[11] = 0x00;

    resp_payload[16] = (ipAddr & 0xFF);         // yiaddr
    resp_payload[17] = (ipAddr >> 8) & 0xFF;
    resp_payload[18] = (ipAddr >> 16) & 0xFF;
    resp_payload[19] = (ipAddr >> 24) & 0xFF;
    memcpy(&resp_payload[28], mac, 6); // Client MAC address

    memcpy(&resp_payload[28], mac, 6); // Client MAC address
    memset(&resp_payload[34], 0, 10);        // Padding to fill the `chaddr` field

    // DHCP Magic Cookie
    resp_payload[236] = 0x63;
    resp_payload[237] = 0x82;
    resp_payload[238] = 0x53;
    resp_payload[239] = 0x63;

    // DHCP Options
    uint8_t constexpr serverIP[] = {192, 168, 27, 1};
    uint16_t offset = 240;

    appendDHCPMsgTypeOption(resp_payload, offset, DHCP_OFFER);
    appendDHCPOption(DHCP_OPTION_SERVER_ID, 4, resp_payload, offset, serverIP);

    uint8_t constexpr leaseTime[] = {0x00, 0x00, 0x70, 0x80};
    appendDHCPOption(DHCP_OPTION_LEASE_TIME, 4, resp_payload, offset, leaseTime);

    uint8_t constexpr subnetMask[] = {255, 255, 255, 0};
    appendDHCPOption(DHCP_OPTION_DNS_SERVER, 4, resp_payload, offset, subnetMask);

    appendDHCPOption(DHCP_OPTION_ROUTER, 4, resp_payload, offset, serverIP);
    appendDHCPOption(DHCP_OPTION_DNS_SERVER, 4, resp_payload, offset, serverIP);

    resp_payload[offset++] = DHCP_OPTION_END; // End Option
    memset(&resp_payload[offset], 0, 4); // Padding

    resp_payload[273] = DHCP_OPTION_END;

    // Send response
    udp_sendto(pcb, response, addr, DHCP_CLIENT_PORT);

    pbuf_free(response);

    logger << "DHCP Offered IP " << ip4addr_ntoa(&ip.value()) << " to MAC " << macAddrToString(mac) << std::endl;

    return false;
}

void DHCPServer::handleDHCPRequest(struct udp_pcb *pcb, const ip_addr_t *addr, uint8_t *payload, uint32_t xid, uint8_t mac[6]) {
    logger << "DHCP type REQUEST" << std::endl;

    // Extract requested IP from Option 50
    uint32_t requested_ip = 0;
    uint16_t options_offset = 240; // Start of DHCP options
    while (options_offset < 300) {
        uint8_t const option = payload[options_offset];
        if (option == 50) { // Requested IP Address
            memcpy(&requested_ip, &payload[options_offset + 2], sizeof(requested_ip));
            break;
        }
        if (option == 255) break; // End Option
        options_offset += 2 + payload[options_offset + 1]; // Skip to next option
    }

    if (requested_ip == 0) {
        logger << "DHCP No requested IP in Option 50, using yiaddr" << std::endl;
        requested_ip = *reinterpret_cast<uint32_t *>(&payload[16]); // Fallback to yiaddr
    }

    // Validate the requested IP
    auto const valid_ip = leasePool.findIP(requested_ip, mac);

    if (!valid_ip) {
        logger << "DHCP Invalid IP " << ip4addr_ntoa(reinterpret_cast<ip4_addr_t *>(&requested_ip)) << " requested by MAC " << macAddrToString(mac) << std::endl;
        return;
    }

    // Create DHCP ACK response
    pbuf *response = pbuf_alloc(PBUF_TRANSPORT, 300, PBUF_RAM);
    auto const resp_payload = static_cast<uint8_t *>(response->payload);
    memset(resp_payload, 0, 300);

    // Fill DHCP ACK
    resp_payload[0] = 2; // Message type: Boot Reply

    resp_payload[1] = 0x01; // Hardware type: Ethernet
    resp_payload[2] = 0x06; // Hardware address length: 6 bytes

    *reinterpret_cast<uint32_t *>(&resp_payload[4]) = htonl(xid); // Transaction ID

    // Copy the flags from the client REQUEST
    resp_payload[10] = payload[10];
    resp_payload[11] = payload[11];

    resp_payload[16] = requested_ip & 0xFF;         // yiaddr
    resp_payload[17] = (requested_ip >> 8) & 0xFF;
    resp_payload[18] = (requested_ip >> 16) & 0xFF;
    resp_payload[19] = (requested_ip >> 24) & 0xFF;
    memcpy(&resp_payload[28], mac, 6); // Client MAC address

    // DHCP Magic Cookie
    resp_payload[236] = 0x63;
    resp_payload[237] = 0x82;
    resp_payload[238] = 0x53;
    resp_payload[239] = 0x63;

    // DHCP Options
    uint8_t constexpr serverIP[] = {192, 168, 27, 1};
    uint16_t offset = 240;

    // Option: DHCP Message Type (ACK)
    appendDHCPMsgTypeOption(resp_payload, offset, DHCP_ACK);

    // Option: Server Identifier (192.168, 27, 1)
    appendDHCPOption(DHCP_OPTION_SERVER_ID, 4, resp_payload, offset, serverIP);

    // Option: IP Address Lease Time (28,800 seconds)
    uint8_t constexpr leaseTime[] = {0x00, 0x00, 0x70, 0x80};
    appendDHCPOption(DHCP_OPTION_LEASE_TIME, 4, resp_payload, offset, leaseTime);

    // Option: Renewal Time (T1) (50% of lease time)
    uint8_t constexpr renewalTime[] = {0x00, 0x00, 0x38, 0x40};
    appendDHCPOption(DHCP_OPTION_T1, 4, resp_payload, offset, renewalTime);

    // Option: Rebinding Time (T2) (87.5% of lease time)
    uint8_t constexpr rebindingTime[] = {0x00, 0x00, 0x61, 0xA8};
    appendDHCPOption(DHCP_OPTION_T2, 4, resp_payload, offset, rebindingTime);

    // Option: Subnet Mask (255, 255, 255, 0)
    uint8_t constexpr subnetMask[] = {255, 255, 255, 0};
    appendDHCPOption(DHCP_OPTION_SUBNET_MASK, 4, resp_payload, offset, subnetMask);

    // Option: Router (192.168.27.1)
    appendDHCPOption(DHCP_OPTION_ROUTER, 4, resp_payload, offset, serverIP);

    // Option: DNS Server (192.168.27.1)
    appendDHCPOption(DHCP_OPTION_DNS_SERVER, 4, resp_payload, offset, serverIP);

    resp_payload[offset++] = DHCP_OPTION_END; // End Option
    memset(&resp_payload[offset], 0, 4); // Padding

    // Send the ACK (broadcast or unicast based on flags)
    if (payload[10] & 0x80) { // Broadcast flag set
        udp_sendto(pcb, response, IP_ADDR_BROADCAST, DHCP_CLIENT_PORT);
    } else {
        udp_sendto(pcb, response, addr, DHCP_CLIENT_PORT);
    }

    pbuf_free(response);

    logger << "DHCP Acknowledged IP " << ip4addr_ntoa(reinterpret_cast<ip4_addr_t *>(&requested_ip)) << " for MAC " << macAddrToString(mac) << std::endl;
}

void DHCPServer::handleDHCPDecline(uint8_t mac[6]) {
    if (options.requestedIP == 0) {
        logger << "DHCP DECLINE received, but no requested IP found." << std::endl;
        return;
    }

    logger << "DHCP type DECLINE (IP " << ipAddrToString(options.requestedIP) << ", MAC " << macAddrToString(mac) << ")" << std::endl;
}

void DHCPServer::handleDHCPRelease(uint8_t mac[6]) {
    logger << "DHCP type RELEASE" << std::endl;

    // Free the IP address
    leasePool.release_ip(mac);
    logger << "DHCP Freed IP (MAC " << macAddrToString(mac) << ")" << std::endl;
}

// Handle incoming DHCP requests
void DHCPServer::dhcp_server_callback(void *arg, udp_pcb *pcb, pbuf *p, const ip_addr_t *addr, u16_t port) {
    auto * const dhcpServer = static_cast<DHCPServer*>(arg);
    auto * const payload = static_cast<uint8_t *>(p->payload);

    // Extract transaction ID (xid) and client MAC address
    uint32_t xid;
    memcpy(&xid, &payload[4], sizeof(xid)); // Copy xid safely
    xid = ntohl(xid); // Convert to host byte order
    std::string const xid_hex = std::format("{:08X}", xid);
    uint8_t mac[6];
    memcpy(mac, &payload[28], 6); // Client MAC address

    dhcpServer->parseDHCPOptions(payload, p->len);
    auto const &options = dhcpServer->options;

    // switch (uint8_t const dhcp_message_type = get_dhcp_message_type(payload, p->len)) {
    switch (options.messageType) {
        case DHCP_DISCOVER:
            dhcpServer->handleDHCPDiscover(pcb, addr, xid, mac);
            break;
        case DHCP_REQUEST:
            dhcpServer->handleDHCPRequest(pcb, addr, payload, xid, mac);
            break;
        case DHCP_DECLINE:
            dhcpServer->handleDHCPDecline(mac);
            break;
        case DHCP_RELEASE:
            dhcpServer->handleDHCPRelease(mac);
            break;
        default:
            logger << "DHCP Unknown request type " << static_cast<int>(options.messageType) << std::endl;
    }

    pbuf_free(p);
}

// Start the DHCP server
void DHCPServer::start() {
    udp_pcb *dhcp_pcb = udp_new();
    udp_bind(dhcp_pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
    udp_recv(dhcp_pcb, dhcp_server_callback, this);
}
