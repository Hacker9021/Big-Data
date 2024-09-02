import os
import re
import threading
from collections import defaultdict, Counter
from heapq import nlargest

# 定义全局锁
lock = threading.Lock()


# 读取words.txt文件中的词汇
def load_words(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        words = set(line.strip() for line in f)
    return words

# 读取文件夹中的所有文件内容
def read_folder(folder_path):
    data = {}
    for file_name in os.listdir(folder_path):
        with open(os.path.join(folder_path, file_name), 'r', encoding='utf-8') as f:
            title = file_name.replace('.txt', '')
            content = f.read()
            data[title] = content
    return data

# Map函数
def map_function(data, words):
    map_output = defaultdict(int)
    for title, content in data.items():
        for word in re.findall(r'\w+', content.lower()):
            if word in words:
                map_output[(title, word)] += 1
    return map_output

# Reduce函数
def reduce_function(map_outputs):
    reduce_output = defaultdict(int)
    for map_output in map_outputs:
        for key, count in map_output.items():
            reduce_output[key] = count
    return reduce_output  # 输出形式：(title, word), count

# 合并词频并返回前1000个高频词
def get_top_1000_words(reduce_output):
    references = defaultdict(list)
    word_count = Counter()
    for (title, word), count in reduce_output.items():
        word_count[word] += count
        references[title].append(word)
    top_1000_words = nlargest(1000, word_count.items(), key=lambda x: x[1])
    top_1000_words_list = [word for word, count in top_1000_words]
    top_1000_references = defaultdict(list)
    for word in top_1000_words_list:
        if word in references:
            # 过滤掉不属于 top_1000_words_list 的元素
            filtered_references = [title for title in references[word] if title in top_1000_words_list]
            top_1000_references[word] = filtered_references
    
    return top_1000_words, top_1000_references

# 处理每个Map节点的线程函数
def map_thread_function(folder_path, words, map_results, index):
    data = read_folder(folder_path)
    result = map_function(data, words)
    with lock:
        map_results[index] = result

# 处理Reduce节点的线程函数
def reduce_thread_function(map_results, reduce_results, index):
    result = reduce_function(map_results)
    with lock:
        reduce_results[index] = result

# 主函数
def main():
    words = load_words('words.txt')

    folder_paths = [f'source_data\\folder_{i}' for i in range(1, 10)]  # 这是一个列表生成式，用来生成包含文件夹路径的列表
    
    map_results = [None] * 9 
    threads = []

    # 启动Map节点线程
    for i, folder_path in enumerate(folder_paths):
        t = threading.Thread(target=map_thread_function, args=(folder_path, words, map_results, i))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()
    
    print(map_results)
    reduce_results = [None] * 3
    threads = []

 
    # 启动Reduce节点线程
    for i in range(3):
        t = threading.Thread(target=reduce_thread_function, args=(map_results, reduce_results, i))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    # 转化为字典
    final_reduce_output = defaultdict(int)
    for reduce_output in reduce_results:
        for key, count in reduce_output.items():
            final_reduce_output[key] = count

    top_1000_words, top_1000_references = get_top_1000_words(final_reduce_output)
    
    # 输出map结果
    with open('map_result.txt', 'w', encoding='utf-8') as f:
        for map_result in map_results:
            for (title, word), count in map_result.items():
                f.write(f'({title}, {word}):{count}\n')
   

    # 输出get_top_1000_words（词频和跳转关系）
    with open('top_1000_words_reduce_result.txt', 'w', encoding='utf-8') as f:
        for word, count in top_1000_words:
            f.write(f'{word}: {count}\n')
    
    with open('top_1000_references.txt', 'w', encoding='utf-8') as f:
        for word, references in top_1000_references.items():
            f.write(f'{word}: {references}\n')
if __name__ == '__main__':
    main()