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

    let colorSpeeds = document.getElementsByClassName("color-speed");
    let colorSpeedTemplate = document.getElementById("color-speed-template");
    for (let colorSpeed of colorSpeeds) {
        let clone = colorSpeedTemplate.content.cloneNode(true);
        colorSpeed.appendChild(clone);
        colorSpeed.querySelector("span").textContent = colorSpeed.dataset.title;

        colorSpeed.querySelector("input[type='color']").id = colorSpeed.id + "-color";
        colorSpeed.querySelector("input[type='range']").id = colorSpeed.id + "-speed";
        colorSpeed.addEventListener("input", crystalDidChange);
    }

    document.querySelector("#crystal-upper-speed").value = crystalData.upper.speed;
    document.querySelector("#crystal-middle-speed").value = crystalData.middle.speed;
    document.querySelector("#crystal-lower-speed").value = crystalData.lower.speed;

    let upDowns = document.getElementsByClassName("up-down");
    let upDownTemplate = document.getElementById("up-down-template");
    for (let upDown of upDowns) {
        let clone = upDownTemplate.content.cloneNode(true);
        upDown.appendChild(clone);
        if (upDown.dataset.title) {
            upDown.querySelector("span").textContent = upDown.dataset.title;
        }

        upDown.querySelector("input[type='range']").id = upDown.id + "-range";
        upDown.addEventListener("input", rainbowDidChange);
    }
}

function snapMin(value, minAllowed) {
    if (Math.abs(value) < minAllowed) {
        value = Math.trunc(2 * value / minAllowed) * minAllowed;
    }

    return value;
}

async function brightnessDidChange(event) {
    event.target.value = snapMin(event.target.value, 40);
    let url = `/brightness?value=${event.target.value}`;
    await fetch(url, {method:'PUT'});
}

const crystalData = {
    upper: {
        color: "ff00d0",
        speed: 0.5
    },
    middle: {
        color: "ff00d0",
        speed: 0.2
    },
    lower: {
        color: "ff00d0",
        speed: 0.3
    }
};

async function crystalDidChange(event) {
    switch (event.target.id) {
        case "crystal-upper-color":
            crystalData.upper.color = event.target.value.substring(1);
            break;
        case "crystal-upper-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.upper.speed = event.target.value;
            break;
        case "crystal-middle-color":
            crystalData.middle.color = event.target.value.substring(1);
            break;
        case "crystal-middle-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.middle.speed = event.target.value;
            break;
        case "crystal-lower-color":
            crystalData.lower.color = event.target.value.substring(1);
            break;
        case "crystal-lower-speed":
            event.target.value = snapMin(event.target.value, 0.1);
            crystalData.lower.speed = event.target.value;
            break;
    }

    await fetch('/crystal', {method: 'PUT', body: JSON.stringify(crystalData)});
}

async function setCrystal(color) {
    let colorInputs = document.querySelectorAll(".color-speed-container > input[type='color']");
    for (let colorInput of colorInputs) {
        colorInput.value = "#" + color;
    }

    crystalData.upper.color = color;
    crystalData.middle.color = color;
    crystalData.lower.color = color;

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
            rainbowData.speed = event.target.value;
            break;
    }

    await fetch('/rainbow', {method: 'PUT', body: JSON.stringify(rainbowData)});
}

async function setRainbow(mode) {
    rainbowData.mode = mode;
    await fetch('/rainbow', {method: 'PUT', body: JSON.stringify(rainbowData)});
}
