import torch.nn as nn

class Trainer:
    def __init__(self, model, *, optimizer, criterion=nn.MSELoss()):
        self.model = model
        self.criterion = criterion
        self.optimizer = optimizer

        self.loss_history = []

    def train(self, X, y, num_epochs=2000):
        for epoch in range(num_epochs):
            self.optimizer.zero_grad()
            y_pred = self.model(X)
            loss = self.criterion(y_pred, y.unsqueeze(-1))
            loss.backward()
            self.optimizer.step()

            if epoch % 10 == 0:
                self.loss_history.append(loss.item())
