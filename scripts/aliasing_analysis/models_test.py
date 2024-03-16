import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np

from models import LutWithMemory
from training import Trainer

import unittest

class TestLutWithMemoryFit(unittest.TestCase):
    def test_tanh_fit(self):
        """Test that LutWithMemory can fit tanh(x) on np.arange data"""

        min_x = -5
        max_x = 5
        memory_depth = 3

        model = LutWithMemory(input_range=(min_x, max_x), bins_num=64, memory_depth=memory_depth)
        criterion = nn.MSELoss()
        optimizer = optim.Adam(model.parameters(), lr=0.01)


        x = torch.arange(min_x, max_x, 0.01)
        y = np.tanh(x)

        X = x.repeat(memory_depth+1, 1)

        # Train
        trainer = Trainer(model, optimizer=optimizer, criterion=criterion)
        trainer.train(X, y, num_epochs=3000)

        # Generate predictions from the model for plotting
        x_eval = torch.arange(min_x, max_x, 0.1)
        X = x_eval.repeat(memory_depth+1, 1)
        y_pred_eval = model(X).detach().squeeze()

        # Convert to numpy for plotting
        x_eval_np = x_eval.numpy()
        y_eval_np = np.tanh(x_eval_np)
        y_pred_eval_np = y_pred_eval.numpy()

        tol = 1e-3
        self.assertTrue(np.allclose(y_eval_np, y_pred_eval_np, atol=tol),
                        msg=f"The prediction error is greater than the threshold {tol}.")

if __name__ == '__main__':
    unittest.main()
