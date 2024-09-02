import networkx as nx
import ast

# 从 TXT 文件读取数据
def read_txt(file_path):
    data = {}
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            parts = line.strip().split(': ')
            label = parts[0]
            items = ast.literal_eval(parts[1])
            data[label] = items
    return data

# 构建有向图
def build_directed_graph(txt_data):
    graph = nx.DiGraph()
    for label, items in txt_data.items():
        for item in items:
            # 添加从标签节点出发的有向边
            graph.add_edge(label, item)
    return graph

# 从 TXT 文件路径读取数据
txt_file_path = 'top_1000_references.txt'
txt_data = read_txt(txt_file_path)

# 构建有向图
directed_graph = build_directed_graph(txt_data)

# 打印有向图信息
print("Number of nodes:", directed_graph.number_of_nodes())
print("Number of edges:", directed_graph.number_of_edges())

# 计算 PageRank
def pagerank_withbeta(graph, damping_factor=0.85, max_iterations=1000, tolerance=1e-8):
    # 初始化 PageRank 值
    pagerank_values = {node: 1 / len(graph) for node in graph.nodes()}
    num_nodes = len(graph)
    
    # 迭代计算 PageRank
    for _ in range(max_iterations):
        new_pagerank_values = {}
        sum_diff = 0
        
        for node in graph.nodes():
            new_pagerank = (1 - damping_factor) / num_nodes
            
            # 考虑所有入边的贡献
            for predecessor in graph.predecessors(node):
                num_out_edges = len(graph.out_edges(predecessor))
                new_pagerank += damping_factor * pagerank_values[predecessor] / num_out_edges
            
            # 计算本次迭代的变化量
            sum_diff += abs(new_pagerank - pagerank_values[node])
            
            new_pagerank_values[node] = new_pagerank
        
        pagerank_values = new_pagerank_values
        
        # 如果变化量小于容差值，则停止迭代
        if sum_diff < tolerance:
            break
    
    return pagerank_values

pagerank_values_withbeta = pagerank_withbeta(directed_graph)
pagerank_values_withoutbeta = nx.pagerank(directed_graph, alpha=1.0)



with open('pagerank_results_withbeta.txt', 'w', encoding='utf-8') as f:
    f.write('r2中因子之和:' + str(sum(pagerank_values_withbeta.values())) + '\n')
    for node, value in pagerank_values_withbeta.items():
        f.write(f"{node}: {value}\n")


with open('pagerank_results_withoutbeta.txt', 'w', encoding='utf-8') as f:
    f.write('r1中因子之和:' + str(sum(pagerank_values_withoutbeta.values())) + '\n')
    for node, value in pagerank_values_withoutbeta.items():
        f.write(f"{node}: {value}\n")
