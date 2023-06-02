package javathings.multiwaytree;

public class MultiwayTree<T> {
    private MultiwayNode<T> root;

    public MultiwayTree() {
        root = new MultiwayNode<>();
    }

    public void insert(T data) {
        insert(root, data);
    }

    private void insert(MultiwayNode<T> node, T data) {

    }
}

class MultiwayNode<T> {
    T data;

}
