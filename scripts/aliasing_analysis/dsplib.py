import numpy as np
from scipy import fft


def calc_spectrum(sig, fs=1):
    S = 20*np.log10(np.abs(fft.rfft(sig)))
    f = fft.rfftfreq(len(sig), d=1/fs)

    return f, S


def generate_time_signal(signal_duration_sec, fs_hz, history_smp_num=0):
    return np.arange(-history_smp_num/fs_hz, signal_duration_sec, 1/fs_hz)
