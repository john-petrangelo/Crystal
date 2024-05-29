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
    setupUpDowns(rainbowDidChange, [0, 0.45, 0.55, 1]);
    setupSliders();

    const crystalAdv = document.querySelector("#crystal-advanced");
    crystalAdv.addEventListener("input", crystalDidChange);
    setupTickmarks(crystalAdv, [0, 0.1, 1]);
    document.querySelector("#crystal-upper-color-input").value = "#" + crystalData.upper_color;
    document.querySelector("#crystal-middle-color-input").value = "#" + crystalData.middle_color;
    document.querySelector("#crystal-lower-color-input").value = "#" + crystalData.lower_color;
    document.querySelector("#crystal-upper-speed-input").value = crystalData.upper_speed;
    document.querySelector("#crystal-middle-speed-input").value = crystalData.middle_speed;
    document.querySelector("#crystal-lower-speed-input").value = crystalData.lower_speed;

    const rbMovement = document.querySelector("#rb-movement");
    rbMovement.addEventListener("input", rainbowDidChange);
    setupTickmarks(rbMovement, [0, 0.45, 0.55, 1]);
    document.querySelector("#rb-movement-input").value = mapValue(rainbowData.speed, -1, 1, 0, 1);

    const wc = document.querySelector("#warp-core");
    wc.addEventListener("input", warpCoreDidChange);
    setupTickmarks(wc, [0, 0.06, 1]);
    document.querySelector("#warp-core-speed-input").value =
        mapValue(warpCoreData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#warp-core-color-input").value = "#" + warpCoreData.color;

    const wcDual = document.getElementById('warp-core-dual');
    wcDual.addEventListener('change', warpCoreDidChange);

    const jlAdv = document.querySelector("#jacobs-ladder-advanced")
    jlAdv.addEventListener("input", jacobsLadderDidChange);
    setupTickmarks(jlAdv, [0, 0.06, 1]);
    document.querySelector("#jl-color-input").value = "#" + jacobsLadderData.color;
    document.querySelector("#jl-speed-input").value = mapValue(jacobsLadderData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#jl-size").value = mapValue(jacobsLadderData.size, 0.0, 1.0, 0.0, 1.0);
    document.querySelector("#jl-jitter-period").value = jacobsLadderData.jitterPeriod;
    document.querySelector("#jl-jitter-size").value = mapValue(jacobsLadderData.jitterSize, 0.0, 0.5, 0.0, 1.0);
}

function setupColorSpeeds() {
    let template = document.getElementById("color-speed-template");
    const colorSpeeds = document.getElementsByClassName('color-speed');
    for (let colorSpeed of colorSpeeds) {
        let clone = template.content.cloneNode(true);
        let container = clone.querySelector(".row");
        colorSpeed.appendChild(container);
        colorSpeed.querySelector(".color").id = colorSpeed.id + "-color";
        colorSpeed.querySelector(".speed").id = colorSpeed.id + "-speed";
    }
}
function setupColors() {
    let template = document.getElementById("color-template");
    const colors = document.getElementsByClassName('color');
    for (let color of colors) {
        let clone = template.content.cloneNode(true);
        color.appendChild(clone);
        color.querySelector("input[type='color']").id = color.id + "-input";
    }
}

function setupSpeeds() {
    const template = document.getElementById("speed-template");
    const speeds = document.getElementsByClassName('speed');
    for (const speed of speeds) {
        const clone = template.content.cloneNode(true);
        speed.appendChild(clone);
        speed.querySelector(".slider").id = speed.id + "-input";
    }
}
function setupUpDowns() {
    const upDownTemplate = document.getElementById("up-down-template");
    const upDowns = document.getElementsByClassName('up-down');
    for (const upDown of upDowns) {
        const clone = upDownTemplate.content.cloneNode(true);
        upDown.appendChild(clone);
        upDown.querySelector(".slider").id = upDown.id + "-input";
    }
}
function setupSliders() {
    let template = document.getElementById("slider-template");
    const sliders = document.getElementsByClassName('slider');
    for (let slider of sliders) {
        let clone = template.content.cloneNode(true);
        slider.appendChild(clone);
        const id = slider.id;
        slider.removeAttribute("id");
        slider.querySelector("input[type='range']").id = id;
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
    const colorInputs = document.querySelectorAll("#crystal-advanced input[type='color']");
    for (const colorInput of colorInputs) {
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
        case "rb-movement-input":
            let denormalizedValue = mapValue(event.target.value, 0, 1, -1, 1);
            denormalizedValue = snapMin(denormalizedValue, 0.1);
            event.target.value = mapValue(denormalizedValue, -1, 1, 0, 1);
            rainbowData.speed = parseFloat(denormalizedValue);
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
    jitterPeriod: 0.15,
    jitterSize: 0.15,
};
async function jacobsLadderDidChange(event) {
    switch (event.target.id) {
        case "jacobs-ladder-color":
            jacobsLadderData.color = event.target.value.substring(1);
            break;
        case "jacobs-ladder-speed":
            jacobsLadderData.frequency = mapValue(event.target.value, 0.0, 1.0, 0.3, 5.3);
            break;
        case "jacobs-ladder-size":
            jacobsLadderData.size = mapValue(event.target.value, 0.0, 1.0, 0.0, 1.0);
            break;
        case "jacobs-ladder-jitter-period":
            jacobsLadderData.jitterPeriod = event.target.value;
            break;
        case "jacobs-ladder-jitter-size":
            jacobsLadderData.jitterSize = mapValue(event.target.value, 0.0, 1.0, 0.0, 0.5);
            break;
        case "jacobs-ladder-squared":
            jacobsLadderData.squared = event.target.checked;
    }
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}

async function setJacobsLadder(event) {
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}
