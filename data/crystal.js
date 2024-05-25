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

    setupColorSpeeds();
    setupColors();
    setupSpeeds();

    const crystalAdv = document.querySelector("#crystal-advanced");
    crystalAdv.addEventListener("input", crystalDidChange);
    setupTickmarks(crystalAdv, [0, 0.1, 1]);
    document.querySelector("#crystal-upper-speed").value = crystalData.upper_speed;
    document.querySelector("#crystal-middle-speed").value = crystalData.middle_speed;
    document.querySelector("#crystal-lower-speed").value = crystalData.lower_speed;
    document.querySelector("#crystal-upper-color").value = crystalData.upper_color;
    document.querySelector("#crystal-middle-color").value = crystalData.middle_color;
    document.querySelector("#crystal-lower-color").value = crystalData.lower_color;

    let rainbowUpDownIDs = ["rb-movement"];
    setupUpDowns(rainbowUpDownIDs, rainbowDidChange, [-1, -0.1, 0.1, 1]);

    const wc = document.querySelector("#warp-core");
    wc.addEventListener("input", warpCoreDidChange);
    setupTickmarks(wc, [0, 0.06, 1]);
    document.querySelector("#warp-core-speed").value =
        mapValue(warpCoreData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#warp-core-color").value = "#" + warpCoreData.color;

    const wcDual = document.getElementById('warp-core-dual');
    wcDual.addEventListener('change', warpCoreDidChange);

    const jlAdv = document.querySelector("#jacobs-ladder-advanced");
    jlAdv.addEventListener("input", jacobsLadderDidChange);

    const jlColor = document.querySelector("#jl-color");
    jlColor.value = "#" + jacobsLadderData.color;

    const jlSpeed = document.querySelector("#jl-speed");
    setupTickmarks(jlSpeed, [0, 0.06, 1]);
    jlSpeed.value = mapValue(jacobsLadderData.frequency, 0.3, 5.3, 0.0, 1.0);

    const jlSquared = document.getElementById('jacobs-ladder-squared');
}

function setupColorSpeeds() {
    let colorSpeedTemplate = document.getElementById("color-speed-template");
    const colorSpeeds = document.getElementsByClassName('color-speed');
    for (let colorSpeed of colorSpeeds) {
        let clone = colorSpeedTemplate.content.cloneNode(true);
        let container = clone.querySelector(".color-speed-container");
        colorSpeed.appendChild(container);
        colorSpeed.querySelector(".color").id = colorSpeed.id + "-color";
        colorSpeed.querySelector(".speed").id = colorSpeed.id + "-speed";
    }
}
function setupColors() {
    let colorTemplate = document.getElementById("color-template");
    const colors = document.getElementsByClassName('color');
    for (let color of colors) {
        let clone = colorTemplate.content.cloneNode(true);
        color.appendChild(clone);
        color.querySelector("input[type='color']").id = color.id + "-input";
    }
}

function setupSpeeds() {
    let speedTemplate = document.getElementById("speed-template");
    const speeds = document.getElementsByClassName('speed');
    for (let speed of speeds) {
        let clone = speedTemplate.content.cloneNode(true);
        speed.appendChild(clone);
        speed.querySelector("input[type='range']").id = speed.id + "-input";
    }
}
function setupTickmarks(parent, tickmarks) {
    const tickmarksID = parent.id + "-tickmarks";

    const datalist = parent.querySelector("datalist");
    datalist.id = tickmarksID;

    const input = parent.querySelector('input[type="range"]')
    input.setAttribute("list", tickmarksID);

    datalist.innerHTML = "";
    for (let tickmark of tickmarks) {
        datalist.innerHTML += `<option value="${tickmark}"></option>`;
    }
}

function setupTickmarks_old(parent, tickmarksId, tickmarks) {
    let dataList = parent.getElementById(tickmarksId);
    dataList.innerHTML = "";
    for (let tickmark of tickmarks) {
        dataList.innerHTML += `<option value="${tickmark}"></option>`;
    }
}

function setupUpDowns(rainbowUpDownIDs, listener, tickmarks) {
    let upDownTemplate = document.getElementById("up-down-template");
    for (let id of rainbowUpDownIDs) {
        let upDown = document.getElementById(id);
        let clone = upDownTemplate.content.cloneNode(true);

        setupTickmarks_old(clone, "up-down-tickmarks", tickmarks);

        upDown.appendChild(clone);
        if (upDown.dataset.title) {
            upDown.querySelector("span").textContent = upDown.dataset.title;
        }

        upDown.querySelector("input[type='range']").id = upDown.id + "-range";
        upDown.addEventListener("input", listener);
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
