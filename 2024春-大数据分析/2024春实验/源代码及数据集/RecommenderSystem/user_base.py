import numpy as np
import pandas as pd
import time

def get_utility_mat() -> np.ndarray:
    """计算效用矩阵

    Returns:
        np.ndarray: 效用矩阵
    """
    # ? 获取用户-动漫评分效用矩阵
    train_file = open('data/train_set.csv', 'r', encoding='utf-8')
    # * train_file: 存放每位用户评论的动漫和评分
    # * train_file 是一个嵌套字典
    train_data = {}
    for line in train_file.readlines()[1:]:
        line = line.strip().split(',')
        # line[0] 为用户 id，line[1] 为动漫 id，line[2] 为评分
        if line[0] not in train_data.keys():
            train_data[line[0]] = {line[1]: line[2]}
        else:
            train_data[line[0]][line[1]] = line[2]
    # * 效用矩阵
    utility_mat = pd.DataFrame(train_data).fillna(0).astype(float)
    return utility_mat

class UserBasedRecommendSystem(object):
    """基于用户的协同过滤推荐系统"""

    def __init__(self, utility_mat):
        self.utility_mat = utility_mat
        self.user_sim_mat = None

    def get_corr_mat(self):
        """计算 Pearson 相关系数矩阵"""
        self.user_sim_mat = self.utility_mat.corr() # [762 rows x 762 columns]

    # TODO k 可以选择
    def predict(self, user_id: int, anime_id: int, k: int = 150) -> float:
        """根据 k 个最相似的用户预测用户 user_id 对动漫 anime_id 的评分

        Args:
            user_id (int): 用户 id
            anime_id (int): 动漫 id
            k (int, optional): 选取的相似用户数量. 默认为 110.

        Returns:
            float: 用户的动漫的预测评分
        """
        user_id = str(user_id)
        anime_id = str(anime_id)
        # 从相关系数矩阵中找到与 user_id 相关的用户
        sim_dict = dict(self.user_sim_mat[user_id])
        sorted_sim_dict = sorted(
            sim_dict.items(), key=lambda x: x[1], reverse=True)   # 按值的大小逆序排序
        # 取 k 个最相似的用户的 id
        top_k_id = [sorted_sim_dict[i][0] for i in range(k)]
        top_k_mat = self.utility_mat[top_k_id]
        # 获得 k 个最相似用户对 anime_id 的评分
        scores = top_k_mat.loc[anime_id]
        pred_score = np.mean(scores[scores != 0])
        return pred_score

    def recommend(self, user_id: str, k: int, n: int) -> None:
        """根据 k 个最相似的用户为用户 user_id 推荐 n 部动漫

        Args:
            user_id (str): 用户 id
            k (int): 选取的相似用户数量
            n (int): 推荐的动漫数量
        """
        # 从相关系数矩阵中找到与 user_id 相关的用户
        sim_dict = dict(self.user_sim_mat[user_id])
        sorted_sim_dict = sorted(
            sim_dict.items(), key=lambda x: x[1], reverse=True)
        # 取 k 个最相似的用户的 id
        top_k_id = [sorted_sim_dict[i][0] for i in range(k)]  #取键
        top_k_mat = self.utility_mat[top_k_id]  # 用户id对应的评分矩阵
        pred_dict = {}
        for i in range(len(self.utility_mat)):
            x = top_k_mat.iloc[i]   # 从 top_k_mat DataFrame 中提取第 i 行，这个 DataFrame 包含了前 k 个最相似项的评分数据。x 是一个 Series 对象，代表了当前动漫的评分数据。
            if len(x[x != 0]) > 20:  # * 某部动漫至少有 20 个相关用户打过分才进行预测
                pred_i = np.mean(x[x != 0])  # 计算x中所有非零元素的算术平均值，即对当前动漫的预测评分
                pred_dict[i] = 0 if np.isnan(pred_i) else pred_i
            else:
                pred_dict[i] = 0
        # 对预测的动漫按照预测分数进行降序排列
        sorted_pred_dict = sorted(
            pred_dict.items(), key=lambda d: d[1], reverse=True)
        # 取前 n 个动漫进行推荐
        pred_res = sorted_pred_dict[:min(n, len(sorted_pred_dict))]
        # 推荐结果
        print("对用户 {} 推荐如下动漫:".format(user_id))
        print("Anime\tScore")
        print("-" * 15)
        for i in range(n):
            idx, score = pred_res[i]
            print("%-6s\t%.3f" % (str(self.utility_mat.index[idx]), score))

if __name__ == "__main__":
    utility_matrix = get_utility_mat()

    # ? 测试数据读取
    test_data = pd.read_csv('data/test_set.csv')
    test_users, test_animes, test_ratings = test_data[
        'user_id'], test_data['anime_id'], test_data['rating'].values

    # * 基础版
    start_time = time.time()

    base_recommender = UserBasedRecommendSystem(utility_matrix)
    base_recommender.get_corr_mat()

    pred_ratings = np.zeros(len(test_data))
    # * 进行预测
    for i in range(len(test_data)):
        pred_ratings[i] = base_recommender.predict(
            test_users[i], test_animes[i])

    # * 计算 SSE
    sse = np.sum(np.square(pred_ratings - test_ratings))
    print("基础版 SSE = ", sse)

    finish_time = time.time()
    # * 用时统计
    print("总时间: {:.3f} s.".format(finish_time - start_time))

    # * 为指定用户进行推荐
    base_recommender.recommend('629', 150, 20)

    print("\n----------------------\n")