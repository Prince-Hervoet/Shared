package javaThings.bst;

/**
 * 树结构
 */
public class BstStruct<K, T> {

    /**
     * 比较器
     */
    static public interface Comparer<K> {
        int compare(K a, K b);
    }

    /**
     * 键值对
     */
    static public class BstPair<K, T> {
        public K key;
        public T value;

        public BstPair(K key, T value) {
            this.key = key;
            this.value = value;
        }
    }

    private BstNode<K, T> root;

    private int size = 0;

    private Comparer<K> comparer;

    BstStruct(Comparer<K> comparer) {
        this.comparer = comparer;
    }

    // ===========================================================================
    // 以下为public
    // ===========================================================================

    public int size() {
        return size;
    }

    public void insert(K key, T data) {
        if (data == null) {
            return;
        }
        root = insert(root, key, data);
    }

    public void remove(K key) {
        if (key == null) {
            return;
        }
        if (!containsKey(key)) {
            return;
        }
        this.size -= 1;
        root = remove(root, key);
    }

    public T get(K key) {
        if (key == null) {
            return null;
        }
        BstNode<K, T> run = root;
        while (run != null) {
            if (comparer.compare(key, run.key) > 0) {
                // 要插入的数据比当前节点的数据大
                run = run.right;
            } else if (comparer.compare(key, run.key) < 0) {
                run = run.left;
            } else {
                return run.data;
            }
        }
        return null;
    }

    public BstPair<K, T> getPair(K key) {
        if (key == null) {
            return null;
        }
        BstNode<K, T> run = root;
        while (run != null) {
            if (comparer.compare(key, run.key) > 0) {
                // 要插入的数据比当前节点的数据大
                run = run.right;
            } else if (comparer.compare(key, run.key) < 0) {
                run = run.left;
            } else {
                return new BstPair<K, T>(key, run.data);
            }
        }
        return null;
    }

    public boolean containsKey(K key) {
        if (key == null) {
            return false;
        }
        BstNode<K, T> run = root;
        while (run != null) {
            if (comparer.compare(key, run.key) > 0) {
                // 要插入的数据比当前节点的数据大
                run = run.right;
            } else if (comparer.compare(key, run.key) < 0) {
                run = run.left;
            } else {
                return true;
            }
        }
        return false;
    }

    public void clear() {
        root = null;
        size = 0;
    }

    // ===========================================================================
    // 以下为private
    // ===========================================================================

    private BstNode<K, T> insert(BstNode<K, T> node, K key, T data) {
        if (node == null) {
            node = new BstNode<>(key, data);
            this.size += 1;
        } else if (comparer.compare(key, node.key) > 0) {
            // 要插入的数据比当前节点的数据大
            node.right = insert(node.right, key, data);
            node = makeBalance(node);
        } else if (comparer.compare(key, node.key) < 0) {
            node.left = insert(node.left, key, data);
            node = makeBalance(node);
        } else {
            node.data = data;
        }
        node.height = Math.max(getHeight(node.left), getHeight(node.right)) + 1;
        return node;
    }

    private BstNode<K, T> remove(BstNode<K, T> node, K key) {
        if (node == null) {
            return node;
        }
        if (comparer.compare(key, node.key) > 0) {
            node.right = remove(node.right, key);
            node = makeBalance(node);
        } else if (comparer.compare(key, node.key) < 0) {
            node.left = remove(node.left, key);
            node = makeBalance(node);
        } else {
            if (node.left == null || node.right == null) {
                BstNode<K, T> temp = node;
                node = node.left == null ? node.right : node.left;
                temp.left = null;
                temp.right = null;
            } else if (node.left != null && node.right != null) {
                if (getFactor(node) > 0) {
                    BstNode<K, T> temp = getMaxNode(node.left);
                    node.key = temp.key;
                    node.data = temp.data;
                    node.left = remove(node.left, temp.key);
                } else {
                    BstNode<K, T> temp = getMinNode(node.right);
                    node.key = temp.key;
                    node.data = temp.data;
                    node.right = remove(node.right, temp.key);
                }
            }
        }
        if (node != null) {
            node.height = Math.max(getHeight(node.left), getHeight(node.right)) + 1;
        }
        return node;
    }

    private BstNode<K, T> singleRight(BstNode<K, T> target) {
        BstNode<K, T> lc = target.left;
        target.left = lc.right;
        lc.right = target;
        lc.height = Math.max(getHeight(lc.left), getHeight(lc.right)) + 1;
        target.height = Math.max(getHeight(target.left), getHeight(target.right)) + 1;
        return lc;
    }

    private BstNode<K, T> singleLeft(BstNode<K, T> target) {
        BstNode<K, T> rc = target.right;
        target.right = rc.left;
        rc.left = target;
        rc.height = Math.max(getHeight(rc.left), getHeight(rc.right)) + 1;
        target.height = Math.max(getHeight(target.left), getHeight(target.right)) + 1;
        return rc;
    }

    private BstNode<K, T> leftRight(BstNode<K, T> target) {
        target.left = singleLeft(target.left);
        return singleRight(target);
    }

    private BstNode<K, T> rightLeft(BstNode<K, T> target) {
        target.right = singleRight(target.right);
        return singleLeft(target);
    }

    private int getHeight(BstNode<K, T> node) {
        if (node == null) {
            return 0;
        }
        return node.height;
    }

    private int getFactor(BstNode<K, T> target) {
        return getHeight(target.left) - getHeight(target.right);
    }

    /**
     * 调平衡
     * 
     * @param target
     * @return
     */
    private BstNode<K, T> makeBalance(BstNode<K, T> target) {
        int ff = getFactor(target);
        if (ff > 1) {
            if (getFactor(target.left) > 0) {
                target = singleRight(target);
            } else {
                target = leftRight(target);
            }
        } else if (ff < -1) {
            if (getFactor(target.right) < 0) {
                target = singleLeft(target);
            } else {
                target = rightLeft(target);
            }
        }
        return target;
    }

    private BstNode<K, T> getMaxNode(BstNode<K, T> node) {
        if (node == null) {
            return null;
        }
        while (node.right != null) {
            node = node.right;
        }
        return node;
    }

    private BstNode<K, T> getMinNode(BstNode<K, T> node) {
        if (node == null) {
            return null;
        }
        while (node.left != null) {
            node = node.left;
        }
        return node;
    }
}

/**
 * BST树节点
 */
class BstNode<K, T> {
    protected K key;
    protected T data;
    protected BstNode<K, T> left, right;
    protected int height;

    BstNode(K key, T data) {
        this.key = key;
        this.data = data;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public void setLeft(BstNode<K, T> left) {
        this.left = left;
    }

    public void setRight(BstNode<K, T> right) {
        this.right = right;
    }

    public BstNode<K, T> getLeft() {
        return left;
    }

    public BstNode<K, T> getRight() {
        return right;
    }

}