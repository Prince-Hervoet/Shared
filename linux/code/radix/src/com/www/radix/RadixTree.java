package com.www.radix;

import java.util.HashMap;
import java.util.Map;

public class RadixTree {

    public TreeNode root;

    private int nodeSize;

    private void initRoot() {
        if (root == null) {
            root = new TreeNode();
            root.children = new HashMap<>();
            nodeSize += 1;
        }
    }

    public void insertValue(String target) {
        initRoot();
        TreeNode run = root;
        int targetIndex = 0;
        boolean hasDiff = false;
        while (targetIndex < target.length()) {
            hasDiff = false;
            TreeNode child = run.children.get(target.charAt(targetIndex));
            if (child == null) {
                child = new TreeNode();
                nodeSize += 1;
                child.value = target.substring(targetIndex);
                child.children = new HashMap<>();
                child.hasData = true;
                run.children.put(target.charAt(targetIndex), child);
                break;
            }

            int patternIndex = 0;
            while (patternIndex < child.value.length() && targetIndex < target.length()) {
                if (child.value.charAt(patternIndex) != target.charAt(targetIndex)) {
                    hasDiff = true;
                    break;
                }
                patternIndex++;
                targetIndex++;
            }

            if (hasDiff) {
                // 出现不同字符，进行节点分裂
                String patternFront = child.value.substring(0, patternIndex);
                String patternEnd = child.value.substring(patternIndex);
                child.value = patternFront;
                child.hasData = false;

                TreeNode newNode = new TreeNode();
                newNode.value = patternEnd;
                newNode.hasData = true;
                newNode.children = child.children;
                child.children = new HashMap<>();
                child.children.put(patternEnd.charAt(0), newNode);

                String targetEnd = target.substring(targetIndex);
                newNode = new TreeNode();
                newNode.value = targetEnd;
                newNode.hasData = true;
                newNode.children = new HashMap<>();
                child.children.put(targetEnd.charAt(0), newNode);
                nodeSize += 2;
                break;
            } else {
                // 没有不同字符，匹配完了

                if (targetIndex == target.length() && patternIndex == child.value.length()) {
                    // 如果要插入的字符串刚好和模式串一起匹配结束，说明插入完成
                    child.hasData = true;
                    break;
                } else if (targetIndex == target.length()) {
                    // 如果插入字符串先匹配结束，说明被覆盖了，则进行分裂
                    String patternFront = child.value.substring(0, patternIndex);
                    String patternEnd = child.value.substring(patternIndex);
                    child.value = patternFront;
                    child.hasData = true;
                    TreeNode newNode = new TreeNode();
                    newNode.value = patternEnd;
                    newNode.hasData = true;
                    newNode.children = child.children;
                    child.children = new HashMap<>();
                    child.children.put(patternEnd.charAt(0), newNode);
                    nodeSize += 1;
                } else if (patternIndex == child.value.length()) {
                    // 如果模式串先匹配结束，说明还不够，则继续往下找
                    run = child;
                }
            }
        }
    }

    public int getNodeSize() {
        return nodeSize;
    }

    public boolean isExist(String str) {
        int strIndex = 0;
        TreeNode run = root;
        while (strIndex < str.length()) {
            TreeNode child = run.children.get(str.charAt(strIndex));
            if (child == null) {
                return false;
            }
            int patternIndex = 0;
            while (patternIndex < child.value.length() && strIndex < str.length()) {
                if (child.value.charAt(patternIndex) != str.charAt(strIndex)) {
                    return false;
                }
                ++patternIndex;
                ++strIndex;
            }
            run = child;
        }
        return run.hasData;
    }

    public void search(String str) {

    }

    public static void main(String[] args) {
        RadixTree rt = new RadixTree();
        rt.insertValue("abcd");
        rt.insertValue("abce");
        rt.insertValue("abcg");
        rt.insertValue("ab");
        System.out.println(rt.isExist("ab"));
    }
}

class TreeNode {
    public String value;
    public boolean hasData;
    public Object data;
    public Map<Character, TreeNode> children;
}