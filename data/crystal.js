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
    brightness.addEventListener("input", throttle(brightnessDidChange, 10 /*ms*/));

    const home = document.getElementById("home");
    const subpages = document.getElementsByClassName("subpage");
    const subpageTopBarTemplate = document.getElementById("subpage-top-bar-template");

    for (const subpage of subpages) {
        const clone = subpageTopBarTemplate.content.cloneNode(true);
        subpage.insertBefore(clone, subpage.firstChild);

        const label = subpage.querySelector(".subpage-title");
        label.textContent = subpage.dataset.title;
    
        const button = document.getElementById(subpage.dataset.buttonid);

        button.onclick = function() {
            home.classList.add("homepage-out");
            subpage.classList.add("subpage-is-open");
        }

        const back_button = subpage.getElementsByClassName("subpage-back")[0];
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
    crystalAdv.addEventListener("input", throttle(crystalDidChange, 10 /*ms*/));
    setupTickmarks(crystalAdv, [0, 0.1, 1]);
    document.querySelector("#crystal-upper-color-input").value = "#" + crystalData.upper_color;
    document.querySelector("#crystal-middle-color-input").value = "#" + crystalData.middle_color;
    document.querySelector("#crystal-lower-color-input").value = "#" + crystalData.lower_color;
    document.querySelector("#crystal-upper-speed-input").value = crystalData.upper_speed;
    document.querySelector("#crystal-middle-speed-input").value = crystalData.middle_speed;
    document.querySelector("#crystal-lower-speed-input").value = crystalData.lower_speed;

    const rbMovement = document.querySelector("#rb-movement");
    rbMovement.addEventListener("input", throttle(rainbowDidChange, 10 /*ms*/));
    setupTickmarks(rbMovement, [0, 0.45, 0.55, 1]);
    document.querySelector("#rb-movement-input").value = mapValue(rainbowData.speed, -1, 1, 0, 1);

    const wc = document.querySelector("#wc");
    wc.addEventListener("input", throttle(warpCoreDidChange, 10 /*ms*/));
    setupTickmarks(wc, [0, 0.06, 1]);
    document.querySelector("#wc-speed-input").value =
        mapValue(warpCoreData.frequency, 0.3, 5.3, 0.0, 1.0);
    document.querySelector("#wc-color-input").value = "#" + warpCoreData.color;

    const wcDual = document.getElementById('wc-dual');
    wcDual.addEventListener('change', throttle(warpCoreDidChange, 10 /*ms*/));

    const jlAdv = document.querySelector("#jacobs-ladder-advanced")
    jlAdv.addEventListener("input", throttle(jacobsLadderDidChange, 10 /*ms*/));
    document.querySelector("#jl-color-input").value = "#" + jacobsLadderData.color;
    document.querySelector("#jl-speed-input").value = mapValue(jacobsLadderData.frequency, 0.3, 2.3, 0.0, 1.0);
    document.querySelector("#jl-size").value = mapValue(jacobsLadderData.size, 0.0, 1.0, 0.0, 1.0);
    document.querySelector("#jl-jitter-period").value = jacobsLadderData.jitterPeriod;
    document.querySelector("#jl-jitter-size").value = mapValue(jacobsLadderData.jitterSize, 0.0, 0.5, 0.0, 1.0);
}

function setupColorSpeeds() {
    const template = document.getElementById("color-speed-template");
    const colorSpeeds = document.getElementsByClassName('color-speed');
    for (const colorSpeed of colorSpeeds) {
        const clone = template.content.cloneNode(true);
        const container = clone.querySelector(".row");
        colorSpeed.appendChild(container);
        colorSpeed.querySelector(".color").id = colorSpeed.id + "-color";
        colorSpeed.querySelector(".speed").id = colorSpeed.id + "-speed";
    }
}
function setupColors() {
    const template = document.getElementById("color-template");
    const colors = document.getElementsByClassName('color');
    for (const color of colors) {
        const clone = template.content.cloneNode(true);
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
    const template = document.getElementById("slider-template");
    const sliders = document.getElementsByClassName('slider');
    for (const slider of sliders) {
        const clone = template.content.cloneNode(true);
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

    const inputs = parent.querySelectorAll('input[type="range"]');
    for (const input of inputs) {
        input.setAttribute("list", tickmarksID);
    }

    datalist.innerHTML = "";
    for (const tickmark of tickmarks) {
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

function logWithTimestamp(...args) {
    const timestamp = new Date().toISOString(); // ISO 8601 format timestamp
    console.log(`[${timestamp}]`, ...args);
}

function throttle(func, delay) {
    let lastCall = 0;
    let timeout = null;

    return (...args) => {
        const now = Date.now();

        const remainingTime = delay - (now - lastCall);
        const execFunc = () => {
            lastCall = Date.now();
            timeout = null;
            func(...args);
        };

        if (remainingTime <= 0) {
            clearTimeout(timeout);
            execFunc();
        } else if (!timeout) {
            timeout = setTimeout(execFunc, remainingTime);
        }
    };
}

let lastValue = null;

async function brightnessDidChange(event) {
    const newValue = snapMin(event.target.value, 40);
    event.target.value = newValue;

    if (newValue === lastValue) {
        return;
    }

    lastValue = newValue;

    const url = `/brightness?value=${event.target.value}`;
    try {
        await fetch(url, { method: "PUT" });
    } catch (error) {
        if (error.name === "AbortError") {
            console.log("Request aborted");
        } else {
            console.error("Fetch error: ", error);
        }
    }
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
        case "crystal-upper-color-input":
            crystalData.upper_color = event.target.value.substring(1);
            break;
        case "crystal-upper-speed-input":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.upper_speed = parseFloat(event.target.value);
            break;
        case "crystal-middle-color-input":
            crystalData.middle_color = event.target.value.substring(1);
            break;
        case "crystal-middle-speed-input":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.middle_speed = parseFloat(event.target.value);
            break;
        case "crystal-lower-color-input":
            crystalData.lower_color = event.target.value.substring(1);
            break;
        case "crystal-lower-speed-input":
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

const rainbowData = {
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

const warpCoreData = {
    frequency: 0.6,
    color: "5f5fff",
    dual: false,
};

async function warpCoreDidChange(event) {
    switch (event.target.id) {
        case "wc-color-input":
            warpCoreData.color = event.target.value.substring(1);
            break;
        case "wc-speed-input":
            warpCoreData.frequency = mapValue(event.target.value, 0.0, 1.0, 0.3, 5.3);
            break;
        case "wc-dual":
            warpCoreData.dual = event.target.checked;
    }
    await fetch('/warpcore', {method: 'PUT', body: JSON.stringify(warpCoreData)});
}

async function setWarpCore() {
    await fetch('/warpcore', {method: 'PUT', body: JSON.stringify(warpCoreData)})
}

const jacobsLadderData = {
    frequency: 0.6,
    size: 0.15,
    color: "ffffff",
    jitterPeriod: 0.15,
    jitterSize: 0.15,
};
async function jacobsLadderDidChange(event) {
    switch (event.target.id) {
        case "jl-color-input":
            jacobsLadderData.color = event.target.value.substring(1);
            break;
        case "jl-speed-input":
            jacobsLadderData.frequency = mapValue(event.target.value, 0.0, 1.0, 0.3, 2.3);
            break;
        case "jl-size":
            jacobsLadderData.size = mapValue(event.target.value, 0.0, 1.0, 0.0, 1.0);
            break;
        case "jl-jitter-period":
            jacobsLadderData.jitterPeriod = event.target.value;
            break;
        case "jl-jitter-size":
            jacobsLadderData.jitterSize = mapValue(event.target.value, 0.0, 1.0, 0.0, 0.5);
            break;
    }
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}

async function setJacobsLadder(event) {
    await fetch('/jacobsladder', {method: 'PUT', body: JSON.stringify(jacobsLadderData)});
}
