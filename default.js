const a = new Promise((resolve, reject) => {
  resolve(123);
}).then(
  (result) => {
    console.log(result);
  },
  (reason) => {}
);

const b = new Promise((resolve, reject) => {
  setTimeout(() => {
    resolve(123123);
  }, 1000);
})
  .then((result) => {
    console.log(result);
  })
  .then()
  .then();

// Promise.all([b, a]).then((result) => {
//   console.log(result);
// });
