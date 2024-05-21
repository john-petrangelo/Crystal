window.addEventListener("load", startup, false);

function startup() {
    const brightness = document.getElementById("brightness");
    const brightnessBox = document.getElementById("brightness-box");
    fetch("/brightness")
      .then(response => {
        if (!response.ok) {
          throw Error(response.statusText);
        }
        return response.json();
      })
      .then(data => { 
          brightness.value = data.value;
          brightnessBox.style.visibility = "visible";
      })
      .catch(() => {
          brightness.value = 255;
          brightnessBox.style.visibility = "visible";
      });
    brightness.addEventListener("input", brightnessDidChange);

    let home = document.getElementById("home");
    let subpages = document.getElementsByClassName("subpage");
    let subpageTopBarTemplate = document.getElementById("subpage-top-bar-template");

    for (let subpage of subpages) {
        let clone = subpageTopBarTemplate.content.cloneNode(true);
        subpage.insertBefore(clone, subpage.firstChild);

        let label = subpage.querySelector(".subpage-title");
        label.textContent = subpage.dataset.title;
    
        let button = document.getElementById(subpage.dataset.buttonid);

        button.onclick = function() {
            home.classList.add("homepage-out");
            subpage.classList.add("subpage-is-open");
        }

        let back_button = subpage.getElementsByClassName("subpage-back")[0];
        back_button.onclick = function() {
            home.classList.remove("homepage-out");
            subpage.classList.remove("subpage-is-open");
        }
    }

    let crystalColorSpeedIDs = ["crystal-upper", "crystal-middle", "crystal-lower"];
    setupColorSpeeds(crystalColorSpeedIDs, crystalDidChange, [0, 0.1, 1]);
    document.querySelector("#crystal-upper-speed").value = crystalData.upper_speed;
    document.querySelector("#crystal-middle-speed").value = crystalData.middle_speed;
    document.querySelector("#crystal-lower-speed").value = crystalData.lower_speed;

    let rainbowUpDownIDs = ["rb-movement"];
    setupUpDowns(rainbowUpDownIDs, rainbowDidChange, [-1, -0.1, 0.1, 1]);

    let warpCoreColorSpeedIDs = ["warp-core"];
    setupColorSpeeds(warpCoreColorSpeedIDs, warpCoreDidChange, [0, 0.06, 1]);
    document.querySelector("#warp-core-speed").value =
        mapValue(warpCoreData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#warp-core-color").value = "#" + warpCoreData.color;
    document.getElementById('warp-core-dual').addEventListener('change', warpCoreDidChange);

    setupColorSpeeds(["jacobs-ladder"], jacobsLadderDidChange, [0, 0.06, 1]);
    document.querySelector("#jacobs-ladder-speed").value =
        mapValue(jacobsLadderData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#jacobs-ladder-color").value = "#" + jacobsLadderData.color;

    setupColors(["jl-color"], jacobsLadderDidChange);
    document.querySelector("#jl-color").value = "#" + jacobsLadderData.color;

    setupSpeeds(["jl-speed"], jacobsLadderDidChange, [0, 0.06, 1]);
    document.querySelector("#jl-speed").value =
        mapValue(jacobsLadderData.frequency, 0.3, 5.3, 0.0, 1.0);

    document.getElementById('jacobs-ladder-squared').addEventListener('change', jacobsLadderDidChange);
}

function setupColors(ids, listener) {
    let colorTemplate = document.getElementById("color-template");
    for (let id of ids) {
        let color = document.getElementById(id);
        let clone = colorTemplate.content.cloneNode(true);
        color.appendChild(clone);

        // TODO Fix the "-color" suffix holdover
        color.querySelector("input[type='color']").id = color.id + "-color";
        color.addEventListener("input", listener);
    }
}
function setupSpeeds(ids, listener, tickmarks) {
    let speedTemplate = document.getElementById("speed-template");
    for (let id of ids) {
        let speed = document.getElementById(id);
        let clone = speedTemplate.content.cloneNode(true);
        setupTickmarks(clone, "speed-tickmarks", tickmarks);
        speed.appendChild(clone);

        // TODO Fix the "-speed" suffix holdover
        // speed.querySelector("input[type='range']").id = speed.id + "-speed";
        // speed.addEventListener("input", listener);
    }
}
function setupColorSpeeds(ids, listener, tickmarks) {
    let colorSpeedTemplate = document.getElementById("color-speed-template");
    for (let id of ids) {
        let colorSpeed = document.getElementById(id);
        let clone = colorSpeedTemplate.content.cloneNode(true);
        setupTickmarks(clone, "color-speed-tickmarks", tickmarks);
        colorSpeed.appendChild(clone);
        colorSpeed.querySelector("span").textContent = colorSpeed.dataset.title;

        colorSpeed.querySelector("input[type='color']").id = colorSpeed.id + "-color";
        colorSpeed.querySelector("input[type='range']").id = colorSpeed.id + "-speed";
        colorSpeed.addEventListener("input", listener);
    }
}
function setupUpDowns(rainbowUpDownIDs, listener, tickmarks) {
    let upDownTemplate = document.getElementById("up-down-template");
    for (let id of rainbowUpDownIDs) {
        let upDown = document.getElementById(id);
        let clone = upDownTemplate.content.cloneNode(true);

        setupTickmarks(clone, "up-down-tickmarks", tickmarks);

        upDown.appendChild(clone);
        if (upDown.dataset.title) {
            upDown.querySelector("span").textContent = upDown.dataset.title;
        }

        upDown.querySelector("input[type='range']").id = upDown.id + "-range";
        upDown.addEventListener("input", listener);
    }
}

function setupTickmarks(parent, tickmarksId, tickmarks) {
    let dataList = parent.getElementById(tickmarksId);
    dataList.innerHTML = "";
    for (let tickmark of tickmarks) {
        dataList.innerHTML += `<option value="${tickmark}"></option>`;
    }
}

function snapMin(value, minAllowed) {
    if (Math.abs(value) < minAllowed) {
        value = Math.trunc(2 * value / minAllowed) * minAllowed;
    }

    return value;
}

function mapValue(value, fromLow, fromHigh, toLow, toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

async function brightnessDidChange(event) {
    event.target.value = snapMin(event.target.value, 40);
    let url = `/brightness?value=${event.target.value}`;
    await fetch(url, {method:'PUT'});
}

const crystalData = {
    upper_color: "ff00d0",
    upper_speed: 0.5,
    middle_color: "ff00d0",
    middle_speed: 0.2,
    lower_color: "ff00d0",
    lower_speed: 0.3,
};

async function crystalDidChange(event) {
    switch (event.target.id) {
        case "crystal-upper-color":
            crystalData.upper_color = event.target.value.substring(1);
            break;
        case "crystal-upper-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.upper_speed = parseFloat(event.target.value);
            break;
        case "crystal-middle-color":
            crystalData.middle_color = event.target.value.substring(1);
            break;
        case "crystal-middle-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.middle_speed = parseFloat(event.target.value);
            break;
        case "crystal-lower-color":
            crystalData.lower_color = event.target.value.substring(1);
            break;
        case "crystal-lower-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.lower_speed = parseFloat(event.target.value);
            break;
    }

    await fetch('/crystal', {method: 'PUT', body: JSON.stringify(crystalData)});
}

async function setCrystal(color) {
    let colorInputs = document.querySelectorAll(".color-speed-container > input[type='color']");
    for (let colorInput of colorInputs) {
        colorInput.value = "#" + color;
    }

    crystalData.upper_color = color;
    crystalData.middle_color = color;
    crystalData.lower_color = color;

    await fetch('/crystal', {method: 'PUT', body: JSON.stringify(crystalData)});
}

let rainbowData = {
    mode: "classic",
    speed: 0.3
};

async function rainbowDidChange(event) {
    switch (event.target.id) {
        case "rb-movement-range":
            event.target.value = snapMin(event.target.value, 0.1);
            rainbowData.speed = parseFloat(event.target.value);
            break;
    }

    await fetch('/rainbow', {method: 'PUT', body: JSON.stringify(rainbowData)});
}

async function setRainbow(mode) {
    rainbowData.mode = mode;
    await fetch('/rainbow', {method: 'PUT', body: JSON.stringify(rainbowData)});
}

let warpCoreData = {
    frequency: 0.6,
    color: "5f5fff",
    dual: false,
};

async function warpCoreDidChange(event) {
    switch (event.target.id) {
        case "warp-core-color":
            warpCoreData.color = event.target.value.substring(1);
            break;
        case "warp-core-speed":
            warpCoreData.frequency = mapValue(event.target.value, 0.0, 1.0, 0.3, 5.3);
            break;
        case "warp-core-dual":
            warpCoreData.dual = event.target.checked;
    }
    await fetch('/warpcore', {method: 'PUT', body: JSON.stringify(warpCoreData)});
}

async function setWarpCore() {
    await fetch('/warpcore', {method: 'PUT', body: JSON.stringify(warpCoreData)})
}

let jacobsLadderData = {
    frequency: 0.6,
    size: 0.15,
    color: "ffffff",
    squared: false,
    jitterSize: 0.15,
    jitterPeriod: 0.15,
};
async function jacobsLadderDidChange(event) {
    switch (event.target.id) {
        case "jacobs-ladder-color":
            jacobsLadderData.color = event.target.value.substring(1);
            break;
        case "jacobs-ladder-speed":
            jacobsLadderData.frequency = mapValue(event.target.value, 0.0, 1.0, 0.3, 5.3);
            break;
        case "jacobs-ladder-squared":
            jacobsLadderData.squared = event.target.checked;
    }
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}

async function setJacobsLadder(event) {
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}
