const themeToggle = document.getElementById("themeToggle")
let isDark = false

const savedTheme = localStorage.getItem("theme")
if (savedTheme) {
  isDark = savedTheme === "dark"
  document.documentElement.setAttribute("data-theme", savedTheme)
}

themeToggle.addEventListener("click", () => {
  isDark = !isDark
  const theme = isDark ? "dark" : "light"

  document.documentElement.setAttribute("data-theme", theme)
  localStorage.setItem("theme", theme)
})

document.addEventListener("DOMContentLoaded", () => {
  const buttons = document.querySelectorAll(".button-grid .btn, .button-grid-small .btn")
  buttons.forEach((button, index) => {
    button.style.animationDelay = `${1 + index * 0.1}s`
  })

  buttons.forEach((button) => {
    button.addEventListener("mouseenter", () => {
      button.style.transform = "scale(1.05)"
    })

    button.addEventListener("mouseleave", () => {
      button.style.transform = "scale(1)"
    })
  })
})

document.addEventListener("click", (e) => {
  if (e.target.classList.contains("btn")) {
    createRipple(e)
  }
})

function createRipple(event) {
  const button = event.currentTarget
  const rect = button.getBoundingClientRect()
  const size = Math.max(rect.width, rect.height)
  const x = event.clientX - rect.left - size / 2
  const y = event.clientY - rect.top - size / 2

  const ripple = document.createElement("span")
  ripple.style.cssText = `
        position: absolute;
        width: ${size}px;
        height: ${size}px;
        left: ${x}px;
        top: ${y}px;
        background: rgba(255, 255, 255, 0.3);
        border-radius: 50%;
        transform: scale(0);
        animation: ripple 0.6s ease-out;
        pointer-events: none;
    `

  button.style.position = "relative"
  button.style.overflow = "hidden"
  button.appendChild(ripple)

  setTimeout(() => {
    ripple.remove()
  }, 600)
}

const style = document.createElement("style")
style.textContent = `
    @keyframes ripple {
        to {
            transform: scale(2);
            opacity: 0;
        }
    }
`
document.head.appendChild(style)
