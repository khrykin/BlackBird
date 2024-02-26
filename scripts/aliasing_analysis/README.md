# Exploring LadderFilter aliasing issue

`nonlinearity.ipynb` demonstrates how `tanh(x)` function produces new
frequency components and how aliasing phenomenon looks like at various
frequencies and magnitudes.

## Configuring Python environment

1. [Install miniconda](https://docs.anaconda.com/free/miniconda/miniconda-install/)

2. Create `black-bird` conda environment:
```
conda env create -f environment.yml
```

3. Activate the environment:
```
conda activate black-bird
```

VSCode is the recommended editor/IDE. 
