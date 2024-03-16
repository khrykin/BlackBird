import torch
import torch.nn as nn

class LutWithMemory(nn.Module):
    def __init__(self, input_range, bins_num=10, memory_depth=3):
        super(LutWithMemory, self).__init__()
        self.embeddings = nn.ModuleList([nn.Embedding(num_embeddings=bins_num, embedding_dim=1) for _ in range(1+memory_depth)])
        self.bins_num = bins_num
        self.memory_depth = memory_depth
        self.min_x = input_range[0]
        self.max_x = input_range[1]

    def forward(self, X):
        X_normalized = (X - self.min_x) / (self.max_x - self.min_x) * (self.bins_num - 1)
        lower_indices = X_normalized.floor().long()
        upper_indices = torch.clamp(lower_indices + 1, max=self.bins_num-1)

        y_pred = torch.zeros((X_normalized.shape[1], 1))

        for mem_ind in range(1+self.memory_depth):
            lower_embeddings = self.embeddings[mem_ind](lower_indices[mem_ind,:])
            upper_embeddings = self.embeddings[mem_ind](upper_indices[mem_ind,:])

            weights = (X_normalized[mem_ind,:] - lower_indices[mem_ind,:].float()).unsqueeze(-1)
            lut_out = lower_embeddings + weights * (upper_embeddings - lower_embeddings)

            y_pred += lut_out * X[mem_ind,:].unsqueeze(-1)

        return y_pred