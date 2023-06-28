class HandlerFuncObj {
    constructor(handlerFunc, args) {
        this.handlerFunc = handlerFunc;
        this.args = args;
    }
}
class Vigilance {
    constructor() {
        this.keyToProxy = new Map();
        this.keyToTarget = new Map();
    }
    addStore(key, store, reaction) {
        if (!key) {
            return false;
        }
        else if (!reaction) {
            this.keyToProxy[key] = store;
            return true;
        }
        const __deps = {};
        const nObject = { __deps, __reaction: Object.assign({}, reaction) };
        for (const key in store) {
            if (reaction.hasOwnProperty(key)) {
                if (typeof store[key] === "function" &&
                    reaction[key] === Vigilance.reactExecutor) {
                    const methodInfo = Object.getOwnPropertyDescriptor(store, key);
                    const origin = methodInfo.value;
                    methodInfo.value = function (...args) {
                        const result = origin.apply(nObject, args);
                        const dep = nObject.__deps[origin.name];
                        dep.forEach((value) => {
                            value.handlerFunc(args);
                        });
                        return result;
                    };
                    Object.defineProperty(nObject, key, methodInfo);
                }
                else if (reaction[key] == Vigilance.reactVariate) {
                    nObject[key] = store[key];
                }
                __deps[key] = [];
            }
            else {
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
                }
                else if (target.__reaction[prop] === Vigilance.reactExecutor) {
                    return false;
                }
                target[prop] = nValue;
                const dep = target.__deps[prop.toString()];
                dep.forEach((value) => {
                    value.handlerFunc(value.args);
                });
                return true;
            },
        });
        this.keyToProxy[key] = proxy;
        this.keyToTarget[key] = nObject;
        return true;
    }
    getStore(key, dependencies, handler, args) {
        if (!key) {
            return null;
        }
        const proxy = this.keyToProxy[key];
        if (!dependencies || dependencies.length === 0) {
            return proxy;
        }
        if (!handler) {
            handler = () => { };
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
Vigilance.reactExecutor = 1;
Vigilance.reactVariate = 2;
const test = new Vigilance();
test.addStore("test", {
    count: 1,
    size: 123,
    func: () => {
        console.log("func run");
    },
}, {
    count: Vigilance.reactVariate,
    size: Vigilance.reactVariate,
    func: Vigilance.reactExecutor,
});
const a = test.getStore("test", ["count"], (x) => {
    console.log("12jklfjasodfij");
}, null);
a.func();
