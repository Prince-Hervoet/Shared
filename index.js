class MyPromise {
  static #PENDING = "pending";
  static #FULFILLED = "fulfilled";
  static #REJECTED = "rejected";
  #status = MyPromise.#PENDING;
  #value;
  #callbackList = [];

  constructor(fn) {
    try {
      fn(this.#resolve, this.#reject);
    } catch (e) {
      this.#reject(e);
    }
  }

  #resolve = (result) => {
    if (this.#status === MyPromise.#PENDING) {
      this.#status = MyPromise.#FULFILLED;
      this.#value = result;
      queueMicrotask(() => {
        this.#callbackList.forEach((cb) => {
          cb.onFulfilled(result);
        });
      });
    }
  };

  #reject = (reason) => {
    if (this.#status === MyPromise.#PENDING) {
      this.#status = MyPromise.#REJECTED;
      this.#value = reason;
      queueMicrotask(() => {
        this.#callbackList.forEach((cb) => {
          cb.onRejected(reason);
        });
      });
    }
  };

  then = (onFulfilled, onRejected) => {
    if (typeof onFulfilled !== "function") {
      onFulfilled = (value) => value;
    }
    if (typeof onRejected !== "function") {
      onRejected = (value) => value;
    }
    const next = new MyPromise((resolve, reject) => {
      switch (this.#status) {
        case MyPromise.#PENDING:
          this.#callbackList.push({
            onFulfilled: (value) => {
              try {
                const result = onFulfilled(value);
                if (result instanceof MyPromise) {
                  result.then(resolve, reject);
                } else {
                  resolve(result);
                }
              } catch (e) {
                reject(e);
              }
            },
            onRejected: (value) => {
              try {
                const result = onRejected(value);
                if (result instanceof MyPromise) {
                  result.then(resolve, reject);
                } else {
                  resolve(result);
                }
              } catch (e) {
                reject(e);
              }
            },
          });
          break;
        case MyPromise.#FULFILLED:
          queueMicrotask(() => {
            try {
              const result = onFulfilled(this.#value);
              if (result instanceof MyPromise) {
                result.then(resolve, reject);
              } else {
                resolve(result);
              }
            } catch (e) {
              reject(e);
            }
          });
          break;
        case MyPromise.#REJECTED:
          queueMicrotask(() => {
            try {
              const result = onRejected(this.#value);
              if (result instanceof MyPromise) {
                result.then(resolve, reject);
              } else {
                resolve(result);
              }
            } catch (e) {
              reject(e);
            }
          });
          break;
      }
    });
    return next;
  };

  static deferred = function () {
    let result = {};
    result.promise = new MyPromise((resolve, reject) => {
      result.resolve = resolve;
      result.reject = reject;
    });
    return result;
  };

  static all = (promises) => {
    if (!promises || !(promises instanceof Array)) {
      return;
    }
    const answers = [];
    let count = 0,
      i = 0;
    return new MyPromise((res, rej) => {
      promises.forEach((promise) => {
        if (promise) {
          if (typeof promise === "function") {
            const result = promise();
            answers[i] = result;
          } else if (promise instanceof MyPromise) {
            promise.then(
              (result) => {
                count++;
                answers[i] = result;
                if (count === promises.length) {
                  res(answers);
                }
              },
              (reason) => {}
            );
          } else {
            answers[i] = promise;
          }
        }
        i++;
      });
    });
  };
}

const b = new MyPromise((res, rej) => {
  res(111);
});
b.then((result) => {
  return new MyPromise((res, rej) => {
    res(99999);
  });
  // return result;
}).then((result) => {
  console.log(result);
});
console.log("my");

module.exports = MyPromise;
