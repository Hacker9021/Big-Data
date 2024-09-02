import numpy as np
import pandas as pd
import time
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
import re

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
    return utility_mat      # [7893 rows x 762 columns]

# 自定义分词器函数
def custom_tokenizer(text):
    # 匹配逗号相隔的词组
    pattern = r'[\w-]+(?:\s[\w-]+)*'  # 匹配由一个或多个单词或连字符组成的词组，逗号相隔
    matches = re.findall(pattern, text)
    return matches

animes = pd.read_csv('data/anime.csv')

tfidf = TfidfVectorizer(tokenizer=custom_tokenizer)
tfidf_matrix = tfidf.fit_transform(animes['Genres'].tolist()).toarray()
print(tfidf_matrix.shape)

class ContentBasedRecommendSystem(object):
    """基于内容的推荐系统"""

    def __init__(self, utility_mat, animes, tfidf_matrix):
        self.utility_mat = utility_mat   # 效用矩阵
        self.animes = animes
        
        # 索引 - Anime_id 映射
        self.index_to_id = dict(enumerate(self.animes['Anime_id']))  # 0 ~ 1   1 ~ 5   2 ~ 6
        # Anime_id - 索引映射
        self.id_to_index = dict(
            zip(self.index_to_id.values(), self.index_to_id.keys()))

        self.tfidf_mat = tfidf_matrix  # tf-idf 特征矩阵
        self.anime_sim_mat = None  # 动漫相似度矩阵

    def get_anime_sim_mat(self):
        """利用余弦相似度计算动漫之间的相似度矩阵"""
        self.anime_sim_mat = cosine_similarity(self.tfidf_mat) # (17561, 17561) 的张量，包括对角线上元素为0

    def get_predict_score(self, rated_score: np.ndarray, rated_id: np.ndarray, anime_id: int) -> float:
        """计算预测值

        Args:
            rated_score (np.ndarray): _description_
            rated_id (np.ndarray): _description_
            anime_id (int): 动漫 id

        Returns:
            float: 预测值
        """
        distances = self.anime_sim_mat[anime_id]  # anime_id 与其它动漫的相似度
        # 计算集合
        computed_dict = {}
        for i in range(len(rated_id)):
            cosine = distances[self.id_to_index[rated_id[i]]]
            if cosine > 0:
                computed_dict[i] = cosine
        if len(computed_dict.keys()):  # 计算集合不为空，则计算加权预测值
            score_sum, sim_sum = 0, 0
            for k, v in computed_dict.items():
                score_sum += rated_score[k] * v
                sim_sum += v
            return score_sum / sim_sum
        else:  # 计算集合为空，则计算平均值
            return np.mean(rated_score)

    def predict(self, user_id: int, anime_id: int) -> float:
        """预测用户 user_id 对动漫 anime_id 的打分

        Args:
            user_id (int): 用户 id
            anime_id (int): 动漫 id

        Returns:
            float: 预测评分
        """
        user_id = str(user_id)
        # 选取用户打过分的动漫
        exist_rating = (self.utility_mat[user_id] != 0)  # exist_rating 是一个布尔数组
        rated = self.utility_mat[user_id][exist_rating]
        # 打过分的所有动漫的分值
        rated_score = np.array(rated.array)
        # 打过分的所有动漫的 id
        rated_id = np.array(rated.index).astype(int)
        return self.get_predict_score(rated_score, rated_id, self.id_to_index[anime_id])

    def recommend(self, user_id: int, k: int) -> None:
        """为用户 user_id 推荐 k 部动漫

        Args:
            user_id (int): 用户 id
            k (int): 推荐动漫数量
        """
        user_id = str(user_id)
        # 选取用户打过分的动漫
        exist_rating = (self.utility_mat[user_id] != 0)
        rated = self.utility_mat[user_id][exist_rating]
        # 打过分的所有动漫的分值
        rated_score = np.array(rated.array)
        # 打过分的所有动漫的 id
        rated_id = np.array(rated.index).astype(int)
        rec_animes = {}
        for i in range(len(self.animes)):
            idx = self.animes['Anime_id'][i]
            if idx not in rated_id:
                rec_animes[idx] = self.get_predict_score(
                    rated_score, rated_id, self.id_to_index[idx])
        # 排序
        rec_animes_items = list(rec_animes.items())
        rec_animes_items.sort(key=lambda x: x[1], reverse=True)
        rec_animes = [(key, value) for key, value in rec_animes_items][:k]
        # 推荐结果
        print("对用户 {} 推荐如下动漫:".format(user_id))
        print("Anime\tScore")
        print("-" * 15)
        for item in rec_animes:
            print("%-6s\t%.3f" % (item[0], item[1]))

if __name__ == "__main__":
    utility_matrix = get_utility_mat()

    # ? 测试数据读取
    test_data = pd.read_csv('data/test_set.csv')
    test_users, test_animes, test_ratings = test_data[
        'user_id'], test_data['anime_id'], test_data['rating'].values

    # * 基础版
    start_time = time.time()

    base_recommender = ContentBasedRecommendSystem(
        utility_matrix, animes, tfidf_matrix)
    base_recommender.get_anime_sim_mat()

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
    base_recommender.recommend(629, 20)

    print("\n----------------------\n")