#include <bits/stdc++.h>
#include <string>
#include <vector>
class Solution {
public:
    bool isMatch(string s, string p) {
        vector<vector<bool>> dp(s.size()+1,vector<bool>(p.size()+1,false));
        dp[0][0] = true;
        for (int i = 0; i<=s.size(); i++){
            for (int j = 1; j<=p.size(); j++){
                if (i==0){
                    if (p[j-1]=='*'){
                        dp[i][j]=true;
                    }
                    continue;
                }
                if (p[j-1]==s[i-1]){
                    dp[i][j] = dp[i-1][j-1];
                }
                else if (p[j-1]=='.'){
                    dp[i][j] = dp[i-1][j-1];
                }
                else if (p[j-1]=='*'){
                    if (p[j-2]==s[i-1] || p[j-2]=='.')//当星号前字母与s一样，还是可以不使用该组合
                        dp[i][j] = dp[i-1][j] || dp[i][j-2];
                    else
                        dp[i][j] = dp[i][j-2];
                }
                else{
                    dp[i][j] = false;
                }
            }
        }
        return dp[s.size()][p.size()];
    }
};

class Solution {
public:
    bool isConvex(vector<vector<int>>& points) {
        int orient = 0;
        size_t n = points.size();
        for (size_t i = 0; i<points.size(); i++){
            int x1 = points[(i+1)%n][0]-points[i][0];
            int y1 = points[(i+1)%n][1]-points[i][1];
            int x2 = points[(i+2)%n][0]-points[(i+1)%n][0];
            int y2 = points[(i+2)%n][1]-points[(i+1)%n][1];
            int cur = x1*y2-x2*y1;
            if (orient == 0){
                orient = cur>0? 1 : -1;
            }

            if (orient*cur<0){
                return false;
            }
        }
        return true;
    }
};

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    int count = 0;
    void traverse(TreeNode* node){
        //叶子节点
        if (!node->left && !node->right){
            node->val = 0;
            return;
        }
        if (node->left) traverse(node->left);
        if (node->right) traverse(node->right);
        if ((node->left && node->left->val == 0) || (node->right && node->right->val == 0)){
            node->val = 2;
            count++;
            return;
        }
        if ((node->left && node->left->val == 2) || (node->right && node->right->val == 2)){
            node->val = 1;
            return;
        }
        //子节点被监控且子节点不是监控点，该节点还未被监控，为0
        node->val = 0;


    }
    int minCameraCover(TreeNode* root) {
        //叶子节点的父节点必须监控 0标记叶子节点,0表示没有被监控
        //子树全部被监控则作为叶子节点
        //叶子节点不可能放监控
        //使用val=1标记已被监控？
        //使用val=2标记监控位置
        //如果子节点有一个2则该节点为1，被监控
        //如果子节点有一个0则该节点为2，必须被监控（贪心）
        if (!root->left && !root->right)    return 1;
        count = 0;
        traverse(root);
        if (root->val==0)//检查root节点是否被监控！！！
            count++;
        return count;

    }
};