/* ************************************************************************
> File Name:     skiplist.h
> Author:        程序员Carl
> 微信公众号:    代码随想录
> Created Time:  Sun Dec  2 19:04:26 2018
> Description:   
 ************************************************************************/

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <ctime>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

//Class template to implement node//Node 节点只负责给出下一层level中该节点的指针
template<typename K, typename V> 
class Node {

public:
    
    Node() {} 

    Node(K k, V v, int); 

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);
    
    // Linear array to hold pointers to next node of different level
    Node<K, V> **forward;//指针数组、、指向数组，、、数组中每个元素指向Node（即当前level的下一个（右边）节点）

    int node_level;

private:
    K key;
    V value;
};

template<typename K, typename V> 
Node<K, V>::Node(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level; 

    // level + 1, because array index is from 0 - level
    this->forward = new Node<K, V>*[level+1];
    
	// Fill forward array with 0(NULL) 
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
};

template<typename K, typename V> 
Node<K, V>::~Node() {
    delete []forward;//像链表一样释放SkipList中全部节点
};

template<typename K, typename V> 
K Node<K, V>::get_key() const {
    return key;
};

template<typename K, typename V> 
V Node<K, V>::get_value() const {
    return value;
};
template<typename K, typename V> 
void Node<K, V>::set_value(V value) {
    this->value=value;
};

// Class template for Skip list
template <typename K, typename V> 
class SkipList {

public: 
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

    bool primary_key;

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:    
    // Maximum level of the skip list 
    int _max_level;

    // current level of skip list 
    int _skip_list_level;

    // pointer to header node 
    Node<K, V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // skiplist current element count
    int _element_count;
};

// create new node 
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V> *n = new Node<K, V>(k, v, level);//delete??
    return n;
}

// Insert given key and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
/* 
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {
    
    mtx.lock();//防止其他读写冲突
    Node<K, V> *current = this->_header;

    // create update array and initialize it 
    // update is a array which puts the node so that the node->forward[i] can be operated later
    Node<K, V> *update[_max_level+1];//全level、、记录那些需要修改node->forward[i]的node
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));  

    // start form highest level（current level） of skip list 
    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) { //第i层没到最右，且键值小于新添加的点、、
            current = current->forward[i]; //在level i 将节点向右移动
        }
        //退出循环时current->forward[i]的键值大于新key、、则需要将新节点添加至current节点下一层右边
        update[i] = current;//update记录？？？
    }

    // reached level 0 and forward pointer to right node, which is desired to insert key.
    current = current->forward[0];

    // if current node have key equal to searched key 则不添加？？？
    if (primary_key){
        if (current != NULL && current->get_key() == key) {
            std::cout << "key: " << key << ", exists" << std::endl;
            mtx.unlock();
            return 1;
        }
    }


    // if current is NULL that means we have reached to end of the level 
    // if current's key is not equal to key that means we have to insert node between update[0] and current node 
    // if (current == NULL || current->get_key() != key ) {
    if (!primary_key || (primary_key && (current == NULL || current->get_key() != key ) ) ) {
        int random_level = 0;
        //非主键模式下，新加入的key已经存在时,设置新节点的level为0，防止后续的查找插入有多余的步骤
        if (!primary_key && current->get_key == key) {
            random_level = 0;//not so random//hhh
            std::cout << "Insert Repeated key:" << key << std::endl;
        }
        else {
            // Generate a random level for node
            random_level = get_random_level();

            // If random level is greater thar skip list's current level整个跳表目前最高level, initialize update value with pointer to header、、调整header位置（永远最高最左）
            if (random_level > _skip_list_level) {
                for (int i = _skip_list_level+1; i < random_level+1; i++) {
                    update[i] = _header;
                }
                _skip_list_level = random_level;//更新目前最高level
            }
        }
        // create new node with random level generated 
        Node<K, V>* inserted_node = create_node(key, value, random_level);
        
        // insert node 
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;//层数小于等于random_level的 都进行插入
        }
        // delete inserted_node; //不能释放内存呀,因为该指针指向内存会一直存在，直到节点被删除或跳表对象被释放
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count ++;
    }
    mtx.unlock();
    return 0;
}

// Display skip list from low level to high level
template<typename K, typename V> 
void SkipList<K, V>::display_list() {

    std::cout << "\n*****Skip List*****"<<"\n"; 
    for (int i = 0; i <= _skip_list_level; i++) {
        Node<K, V> *node = this->_header->forward[i]; 
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// Dump data in memory to file 
template<typename K, typename V> 
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0]; //最底层包含全部节点

    while (node != NULL) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return ;
}

// Load data from disk
template<typename K, typename V> 
void SkipList<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);

        std::cout << "key:" << *key << "value:" << *value << std::endl;

        //这里应该需要delete？
        delete[] key;
        delete[] value;
        std::cout << "successfully delete" << std::endl;
    }
    _file_reader.close();
}

// Get current SkipList size 
template<typename K, typename V> 
int SkipList<K, V>::size() { 
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());//str.length()超出范围，则substr只复制到string结尾
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

//delete 需要修改， 非主键时一次性删除全部
// Delete element from skip list 
template<typename K, typename V> 
void SkipList<K, V>::delete_element(K key) {

    mtx.lock();
    Node<K, V> *current = this->_header; 
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;//?
    }

    current = current->forward[0];//底层右移，找到要删除的node
    if (current != NULL && current->get_key() == key) {
       
        // start for lowest level and delete the current node of each level
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next node is not target node, break the loop.
            if (update[i]->forward[i] != current) //记录的i层的update[i]->forward[i]不等于要删除节点，说明此层高度大于要删除节点高度
                break;

            update[i]->forward[i] = current->forward[i];
        }

        delete current;
        std::cout << "Successfully deleted current memory "<< key << std::endl;

        // Remove levels which have no elements、、当前最高层的header右边就是null，则此层为空，需要删除
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {//每个节点（包括header）初始化的内存为0（作为NULL）
            _skip_list_level --; 
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        _element_count --;
    }
    mtx.unlock();
    return;
}

// Search for element in skip list 
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V> 
bool SkipList<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    Node<K, V> *current = _header;

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right node, which we search
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

// construct skip list
template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level) {

    this->primary_key = false;//非主键模式

    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    // create header node and initialize key and value to null
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
};

template<typename K, typename V> 
SkipList<K, V>::~SkipList() {
    // printf("SkipList Deconstruction\n");
    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}
//这个随机生成level数是不是有些草率
template<typename K, typename V>
int SkipList<K, V>::get_random_level(){

    int k = 1;
    //rand()不需要参数，它会返回一个从0到最大随机数的任意整数
    while (rand() % 2) {
        k++;
        if (k == _max_level)
            break;
    }
    // k = (k < _max_level) ? k : _max_level;
    return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
