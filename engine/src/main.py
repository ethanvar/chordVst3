import argparse
import random
import os
from typing import Tuple
import torch
from torch.utils.data import Dataset, DataLoader

#!/usr/bin/env python3
"""
PyTorch skeleton training script.
Save as: /home/ethan/Projects/chordVst3/engine/src/main.py
"""


import torch.nn as nn
import torch.optim as optim


# Simple configurable feed-forward network
class SimpleNet(nn.Module):
    def __init__(self, input_dim: int, hidden_dims=(128, 64), out_dim: int = 10):
        super().__init__()
        layers = []
        prev = input_dim
        for h in hidden_dims:
            layers.append(nn.Linear(prev, h))
            layers.append(nn.ReLU(inplace=True))
            prev = h
        layers.append(nn.Linear(prev, out_dim))
        self.net = nn.Sequential(*layers)

    def forward(self, x):
        return self.net(x)


# Placeholder synthetic dataset (replace with real dataset loader)
class RandomDataset(Dataset):
    def __init__(self, size: int, input_dim: int, num_classes: int):
        self.size = size
        self.input_dim = input_dim
        self.num_classes = num_classes

    def __len__(self):
        return self.size

    def __getitem__(self, idx):
        x = torch.randn(self.input_dim, dtype=torch.float32)
        y = torch.randint(0, self.num_classes, (1,)).item()
        return x, y


def set_seed(seed: int):
    random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)


def train_one_epoch(model: nn.Module, loader: DataLoader, optimizer, criterion, device: torch.device):
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0
    for x, y in loader:
        x = x.to(device)
        y = y.to(device)
        preds = model(x)
        loss = criterion(preds, y)
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        running_loss += loss.item() * x.size(0)
        _, p = preds.max(1)
        correct += (p == y).sum().item()
        total += x.size(0)
    return running_loss / total, correct / total


def evaluate(model: nn.Module, loader: DataLoader, criterion, device: torch.device):
    model.eval()
    running_loss = 0.0
    correct = 0
    total = 0
    with torch.no_grad():
        for x, y in loader:
            x = x.to(device)
            y = y.to(device)
            preds = model(x)
            loss = criterion(preds, y)
            running_loss += loss.item() * x.size(0)
            _, p = preds.max(1)
            correct += (p == y).sum().item()
            total += x.size(0)
    return running_loss / total, correct / total


def parse_args():
    p = argparse.ArgumentParser(description="PyTorch skeleton")
    p.add_argument("--epochs", type=int, default=10)
    p.add_argument("--batch-size", type=int, default=64)
    p.add_argument("--lr", type=float, default=1e-3)
    p.add_argument("--seed", type=int, default=42)
    p.add_argument("--input-dim", type=int, default=32)
    p.add_argument("--num-classes", type=int, default=10)
    p.add_argument("--hidden", nargs="+", type=int, default=[128, 64])
    p.add_argument("--save", type=str, default="checkpoint.pth")
    p.add_argument("--device", type=str, default="cuda" if torch.cuda.is_available() else "cpu")
    return p.parse_args()


def main():
    args = parse_args()
    set_seed(args.seed)
    device = torch.device(args.device)

    # model, loss, optimizer
    model = SimpleNet(input_dim=args.input_dim, hidden_dims=tuple(args.hidden), out_dim=args.num_classes)
    model = model.to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=args.lr)

    # data (replace RandomDataset with real dataset)
    train_ds = RandomDataset(size=2000, input_dim=args.input_dim, num_classes=args.num_classes)
    val_ds = RandomDataset(size=500, input_dim=args.input_dim, num_classes=args.num_classes)
    train_loader = DataLoader(train_ds, batch_size=args.batch_size, shuffle=True, num_workers=0)
    val_loader = DataLoader(val_ds, batch_size=args.batch_size, shuffle=False, num_workers=0)

    best_val_acc = 0.0
    for epoch in range(1, args.epochs + 1):
        train_loss, train_acc = train_one_epoch(model, train_loader, optimizer, criterion, device)
        val_loss, val_acc = evaluate(model, val_loader, criterion, device)
        print(f"Epoch {epoch:02d}: train_loss={train_loss:.4f} train_acc={train_acc:.4f} | val_loss={val_loss:.4f} val_acc={val_acc:.4f}")

        # save best
        if val_acc > best_val_acc:
            best_val_acc = val_acc
            state = {
                "epoch": epoch,
                "model_state": model.state_dict(),
                "optimizer_state": optimizer.state_dict(),
                "val_acc": val_acc,
            }
            torch.save(state, args.save)

    print("Training complete. Best val acc:", best_val_acc)


if __name__ == "__main__":
    main()