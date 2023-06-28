type HandlerFunc = (x: any) => void;

class HandlerFuncObj {
  public handlerFunc: HandlerFunc;
  public args: any;

  constructor(handlerFunc: HandlerFunc, args: any) {
    this.handlerFunc = handlerFunc;
    this.args = args;
  }
}

export class Vigilance {
  public static reactExecutor = 1;
  public static reactVariate = 2;

  private keyToProxy: Map<string, object> = new Map();
  private keyToTarget: Map<string, object> = new Map();

  public addStore(key: string, store: object, reaction: object): boolean {
    if (!key) {
      return false;
    } else if (!reaction) {
      this.keyToProxy[key] = store;
      return true;
    }
    const __deps: any = {};
    const nObject: any = { __deps, __reaction: { ...reaction } };

    for (const key in store) {
      if (reaction.hasOwnProperty(key)) {
        if (
          typeof store[key] === "function" &&
          reaction[key] === Vigilance.reactExecutor
        ) {
          const methodInfo: any = Object.getOwnPropertyDescriptor(store, key);
          const origin = methodInfo.value;
          methodInfo.value = function (...args: any[]) {
            const result = origin.apply(nObject, args);
            const dep = nObject.__deps[origin.name];
            dep.forEach((value: HandlerFuncObj) => {
              value.handlerFunc(args);
            });
            return result;
          };
          Object.defineProperty(nObject, key, methodInfo);
        } else if (reaction[key] == Vigilance.reactVariate) {
          nObject[key] = store[key];
        }
        __deps[key] = [];
      } else {
        // 如果store中的字段不在响应式列表中，直接添加
        nObject[key] = store[key];
      }
    }

    const proxy = new Proxy(nObject, {
      get(target, prop) {
        if (!prop || prop.toString().startsWith("__")) {
          return null;
        }
        return target[prop];
      },

      set(target, prop, nValue) {
        if (!prop || prop.toString().startsWith("__")) {
          return false;
        } else if (target.__reaction[prop] === Vigilance.reactExecutor) {
          return false;
        }

        target[prop] = nValue;
        const dep = target.__deps[prop.toString()];
        dep.forEach((value: HandlerFuncObj) => {
          value.handlerFunc(value.args);
        });

        return true;
      },
    });
    this.keyToProxy[key] = proxy;
    this.keyToTarget[key] = nObject;
    return true;
  }

  public getStore(
    key: string,
    dependencies: string[],
    handler: HandlerFunc,
    args: any
  ) {
    if (!key) {
      return null;
    }

    const proxy = this.keyToProxy[key];
    if (!dependencies || dependencies.length === 0) {
      return proxy;
    }

    if (!handler) {
      handler = () => {};
      args = null;
    }
    const han = new HandlerFuncObj(handler, args);
    const target = this.keyToTarget[key];
    dependencies.forEach((value) => {
      target.__deps[value].push(han);
    });

    const re = target.__reaction;
    for (const key in re) {
      if (re[key] === Vigilance.reactExecutor) {
        target.__deps[key].push(han);
      }
    }
    return proxy;
  }
}

const test: any = new Vigilance();
test.addStore(
  "test",
  {
    count: 1,
    size: 123,
    func: () => {
      console.log("func run");
    },
  },
  {
    count: Vigilance.reactVariate,
    size: Vigilance.reactVariate,
    func: Vigilance.reactExecutor,
  }
);

const a = test.getStore(
  "test",
  ["count"],
  (x: any) => {
    console.log("12jklfjasodfij");
  },
  null
);

a.func();
