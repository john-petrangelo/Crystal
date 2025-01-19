#include <cstring>
#include <format>
#include <iomanip>

#include "lwip/udp.h"
#include "lwip/ip4_addr.h"

#include <lumos-arduino/Logger.h>

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

struct Lease {
    ip4_addr_t ip;
    uint8_t mac[6]; // MAC address of the client
    bool active;
};

// IP pool for DHCP server
Lease ip_pool[] = {
    {IPADDR4_INIT_BYTES(192, 168, 27, 2), {0}, false},
    {IPADDR4_INIT_BYTES(192, 168, 27, 3), {0}, false},
    {IPADDR4_INIT_BYTES(192, 168, 27, 4), {0}, false},
    // Add more IPs as needed
};

// Allocate an IP address from the pool
Lease* allocate_ip(uint8_t mac[6]) {
    // First, check if the MAC already has an assigned IP
    for (int i = 0; i < sizeof(ip_pool) / sizeof(ip_pool[0]); i++) {
        if (ip_pool[i].active && memcmp(ip_pool[i].mac, mac, 6) == 0) {
            return &ip_pool[i]; // Return the existing lease
        }
    }

    // If no existing lease, assign a new IP
    for (int i = 0; i < sizeof(ip_pool) / sizeof(ip_pool[0]); i++) {
        if (!ip_pool[i].active) {
            memcpy(ip_pool[i].mac, mac, 6);
            ip_pool[i].active = true;
            return &ip_pool[i];
        }
    }
    return nullptr; // No available IPs
}

// Release an IP address back to the pool
void release_ip(uint8_t mac[6]) {
    for (int i = 0; i < sizeof(ip_pool) / sizeof(ip_pool[0]); i++) {
        if (ip_pool[i].active && memcmp(ip_pool[i].mac, mac, 6) == 0) {
            ip_pool[i].active = false;
            memset(ip_pool[i].mac, 0, 6);
        }
    }
}

std::string_view macStr(uint8_t mac[6]) {
    std::ostringstream oss;
    for (size_t i = 0; i < 6; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
        if (i < 5) {
            oss << ':'; // Add colon separator except for the last byte
        }
    }

    uint8_t ghostMac[6] =         {0x5e, 0x04, 0xc0, 0x31, 0x61, 0x36};
    uint8_t purplePhoenexMac[6] = {0xa2, 0x32, 0xc1, 0xfa, 0x01, 0x70};

    if (memcmp(mac, ghostMac, 6) == 0) {
        oss << " (Ghost)";
    } else if (memcmp(mac, purplePhoenexMac, 6) == 0) {
        oss << " (PurplePhoenix)";
    }

    return oss.str();
}

uint8_t get_dhcp_message_type(uint8_t *payload, uint16_t len) {
    // DHCP options start at offset 240 in the packet
    uint16_t options_offset = 240;
    while (options_offset < len) {
        uint8_t option = payload[options_offset];
        if (option == 255) { // End of options
            break;
        }
        if (option == 53) { // DHCP Message Type
            return payload[options_offset + 2]; // Value is at offset +2
        }
        options_offset += 2 + payload[options_offset + 1]; // Skip to the next option
    }
    return 0; // Return 0 if no valid message type is found
}

void handleDHCPRequest(struct udp_pcb *pcb, const ip_addr_t *addr, uint8_t *payload, uint32_t xid, uint8_t mac[6]) {
    // Handle DHCP REQUEST
    logger << "DHCP type REQUEST" << std::endl;

    // Extract requested IP from Option 50
    uint32_t requested_ip = 0;
    uint16_t options_offset = 240; // Start of DHCP options
    while (options_offset < 300) {
        uint8_t option = payload[options_offset];
        if (option == 50) { // Requested IP Address
            memcpy(&requested_ip, &payload[options_offset + 2], sizeof(requested_ip));
            break;
        }
        if (option == 255) break; // End Option
        options_offset += 2 + payload[options_offset + 1]; // Skip to next option
    }

    if (requested_ip == 0) {
        logger << "No requested IP in Option 50, using yiaddr" << std::endl;
        requested_ip = *(uint32_t *)&payload[16]; // Fallback to yiaddr
    }

    // Validate the requested IP
    bool valid_ip = false;
    for (int i = 0; i < sizeof(ip_pool) / sizeof(ip_pool[0]); i++) {
        if (ip_pool[i].active && ip_pool[i].ip.addr == requested_ip && memcmp(ip_pool[i].mac, mac, 6) == 0) {
            valid_ip = true;
            break;
        }
    }

    if (!valid_ip) {
        logger << "Invalid IP " << ip4addr_ntoa((ip4_addr_t *)&requested_ip) << " requested by MAC " << macStr(mac) << std::endl;
        return;
    }

    // Create DHCP ACK response
    struct pbuf *response = pbuf_alloc(PBUF_TRANSPORT, 300, PBUF_RAM);
    uint8_t *resp_payload = (uint8_t *)response->payload;
    memset(resp_payload, 0, 300);

    // Fill DHCP ACK
    resp_payload[0] = 2; // Message type: Boot Reply
    *(uint32_t *)&resp_payload[4] = htonl(xid); // Transaction ID

    // Copy the flags from the client REQUEST
    resp_payload[10] = payload[10];
    resp_payload[11] = payload[11];

    resp_payload[16] = requested_ip & 0xFF;         // yiaddr
    resp_payload[17] = (requested_ip >> 8) & 0xFF;
    resp_payload[18] = (requested_ip >> 16) & 0xFF;
    resp_payload[19] = (requested_ip >> 24) & 0xFF;
    memcpy(&resp_payload[28], mac, 6); // Client MAC address

    // DHCP Options
    uint16_t offset = 240;

    // Option: DHCP Message Type (ACK)
    resp_payload[offset++] = 53; // Option: DHCP Message Type
    resp_payload[offset++] = 1;  // Length
    resp_payload[offset++] = 5;  // ACK

    // Option: Server Identifier
    resp_payload[offset++] = 54; // Server Identifier
    resp_payload[offset++] = 4;
    resp_payload[offset++] = 192; // 192.168.27.1
    resp_payload[offset++] = 168;
    resp_payload[offset++] = 27;
    resp_payload[offset++] = 1;

    // Option: IP Address Lease Time
    resp_payload[offset++] = 51; // IP Address Lease Time
    resp_payload[offset++] = 4;
    resp_payload[offset++] = 0x00; // 8 hours = 28800 seconds
    resp_payload[offset++] = 0x00;
    resp_payload[offset++] = 0x70;
    resp_payload[offset++] = 0x80;

    // Option: Renewal Time (T1)
    resp_payload[offset++] = 58; // Renewal Time (T1)
    resp_payload[offset++] = 4;
    resp_payload[offset++] = 0x00; // 50% of lease time
    resp_payload[offset++] = 0x00;
    resp_payload[offset++] = 0x38;
    resp_payload[offset++] = 0x40;

    // Option: Rebinding Time (T2)
    resp_payload[offset++] = 59; // Rebinding Time (T2)
    resp_payload[offset++] = 4;
    resp_payload[offset++] = 0x00; // 87.5% of lease time
    resp_payload[offset++] = 0x00;
    resp_payload[offset++] = 0x61;
    resp_payload[offset++] = 0xA8;

    // Option: Subnet Mask
    resp_payload[offset++] = 1;  // Option
    resp_payload[offset++] = 4;  // Length
    resp_payload[offset++] = 255;
    resp_payload[offset++] = 255;
    resp_payload[offset++] = 255;
    resp_payload[offset++] = 0;

    // Option: Router
    resp_payload[offset++] = 3;  // Option
    resp_payload[offset++] = 4;  // Length
    resp_payload[offset++] = 192;
    resp_payload[offset++] = 168;
    resp_payload[offset++] = 27;
    resp_payload[offset++] = 1;

    // Option: DNS Server
    resp_payload[offset++] = 6;  // Option
    resp_payload[offset++] = 4;  // Length
    resp_payload[offset++] = 192;
    resp_payload[offset++] = 168;
    resp_payload[offset++] = 27;
    resp_payload[offset++] = 1;

    resp_payload[offset++] = 255; // End Option
    memset(&resp_payload[offset], 0, 4); // Padding

    // Send the ACK (broadcast or unicast based on flags)
    if (payload[10] & 0x80) { // Broadcast flag set
        udp_sendto(pcb, response, IP_ADDR_BROADCAST, DHCP_CLIENT_PORT);
    } else {
        udp_sendto(pcb, response, addr, DHCP_CLIENT_PORT);
    }

    pbuf_free(response);

    logger << "Acknowledged IP " << ip4addr_ntoa((ip4_addr_t *)&requested_ip) << " for MAC " << macStr(mac) << std::endl;
}

bool handleDHCPDiscover(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint32_t xid, uint8_t mac[6]) {
    logger << "DHCP type DISCOVER" << std::endl;
    Lease *lease = allocate_ip(mac);
    if (!lease) {
        logger << "No IP available for " << macStr(mac) << std::endl << std::endl;
        pbuf_free(p);
        return true;
    }

    // Create DHCP OFFER response
    struct pbuf *response = pbuf_alloc(PBUF_TRANSPORT, 300, PBUF_RAM);
    uint8_t *resp_payload = (uint8_t *)response->payload;
    memset(resp_payload, 0, 300);

    resp_payload[0] = 2; // Message type: Boot Reply

    resp_payload[1] = 0x01; // Hardware type: Ethernet
    resp_payload[2] = 0x06; // Hardware address length: 6

    *(uint32_t *)&resp_payload[4] = htonl(xid); // Transaction ID

    resp_payload[10] = 0x00; // Unicast
    resp_payload[11] = 0x00;

    resp_payload[16] = (lease->ip.addr & 0xFF);         // yiaddr
    resp_payload[17] = (lease->ip.addr >> 8) & 0xFF;
    resp_payload[18] = (lease->ip.addr >> 16) & 0xFF;
    resp_payload[19] = (lease->ip.addr >> 24) & 0xFF;
    memcpy(&resp_payload[28], mac, 6); // Client MAC address

    memcpy(&resp_payload[28], mac, 6); // Client MAC address
    memset(&resp_payload[34], 0, 10);        // Padding to fill the `chaddr` field

    // DHCP Magic Cookie
    resp_payload[236] = 0x63;
    resp_payload[237] = 0x82;
    resp_payload[238] = 0x53;
    resp_payload[239] = 0x63;

    // DHCP Options
    resp_payload[240] = 53;  // DHCP Message Type
    resp_payload[241] = 1;

    resp_payload[242] = 2;   // DHCP OFFER
    resp_payload[243] = 54;  // Server Identifier
    resp_payload[244] = 4;
    resp_payload[245] = 192; // 192.168.27.1
    resp_payload[246] = 168;
    resp_payload[247] = 27;
    resp_payload[248] = 1;

    resp_payload[249] = 51;  // IP Address Lease Time
    resp_payload[250] = 4;
    resp_payload[251] = 0x00; // 8 hours
    resp_payload[252] = 0x00;
    resp_payload[253] = 0x70;
    resp_payload[254] = 0x80;

    resp_payload[255] = 1;   // Subnet Mask
    resp_payload[256] = 4;
    resp_payload[257] = 255; // 255.255.255.0
    resp_payload[258] = 255;
    resp_payload[259] = 255;
    resp_payload[260] = 0;

    resp_payload[261] = 3;   // Router
    resp_payload[262] = 4;
    resp_payload[263] = 192; // 192.168.27.1
    resp_payload[264] = 168;
    resp_payload[265] = 27;
    resp_payload[266] = 1;

    resp_payload[267] = 6;   // Domain Name Server
    resp_payload[268] = 4;
    resp_payload[269] = 192; // 192.168.27.1
    resp_payload[270] = 168;
    resp_payload[271] = 27;
    resp_payload[272] = 1;

    resp_payload[273] = 255; // End Option

    // Send response
    udp_sendto(pcb, response, addr, DHCP_CLIENT_PORT);

    pbuf_free(response);

    logger << "Offered IP " << ip4addr_ntoa(&lease->ip) << " to MAC " << macStr(mac) << std::endl;
    return false;
}

// Handle incoming DHCP requests
void dhcp_server_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    uint8_t *payload = (uint8_t *)p->payload;

    // Extract transaction ID (xid) and client MAC address
    // uint32_t const rawXID = *reinterpret_cast<uint32_t *>(&payload[4]);
    uint32_t xid;
    memcpy(&xid, &payload[4], sizeof(xid)); // Copy xid safely
    xid = ntohl(xid); // Convert to host byte order
    std::string const xid_hex = std::format("{:08X}", xid);
    uint8_t mac[6];
    memcpy(mac, &payload[28], 6); // Client MAC address

    logger << "DHCP received " << p->len << "bytes, xid=" << xid_hex << " mac=" << macStr(mac) << std::endl;

    uint8_t dhcp_message_type = get_dhcp_message_type(payload, p->len);
    if (dhcp_message_type == 1) {
        // Handle DHCP DISCOVER
        if (handleDHCPDiscover(pcb, p, addr, xid, mac)) return;
    } else if (dhcp_message_type == 3) {
        handleDHCPRequest(pcb, addr, payload, xid, mac);
    } else if (dhcp_message_type == 5) {
        // Handle DHCP RELEASE
        logger << "DHCP type RELEASE" << std::endl;

        // Free the IP address
        release_ip(mac);
        logger << "Freed IP for MAC " << macStr(mac) << std::endl;
    } else {
        logger << "DHCP Unknown request type " << static_cast<int>(dhcp_message_type) << std::endl;
    }

    logger << "DHCP Ending request callback" << std::endl << std::endl;
    pbuf_free(p);
}

// Start the DHCP server
void start_dhcp_server() {
    // logger << "Not starting DHCP server, commented out" << std::endl;

    struct udp_pcb *dhcp_pcb = udp_new();
    udp_bind(dhcp_pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
    udp_recv(dhcp_pcb, dhcp_server_callback, NULL);
}
