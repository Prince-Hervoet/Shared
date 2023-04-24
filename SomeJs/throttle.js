function throttle(targetFunc, waitTime) {
  let isSend = true;
  return function () {
    if (isSend) {
      targetFunc();
      isSend = false;
      setTimeout(() => {
        isSend = true;
      }, waitTime ?? 100);
    }
  };
}

function debounce(targetFunc, waitTime) {
  let timer;
  return function () {
    if (timer) {
      clearTimeout(timer);
    }
    timer = setTimeout(() => {
      targetFunc();
    }, waitTime ?? 100);
  };
}

function send() {
  console.log("asdfasfd发送了");
}

const throttleSend = throttle(send, 1000);
const debounceSend = debounce(send, 1000);

const btn = document.getElementById("testBtn");
btn.addEventListener("click", () => {
  debounceSend();
});
