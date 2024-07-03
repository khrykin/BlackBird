import numpy as np
from scipy import fft


def calc_smp_num(signal_duration_sec, fs_hz):
    return int(signal_duration_sec*fs_hz)


def generate_time_signal(signal_duration_sec, fs_hz, history_smp_num=0):
    return np.arange(-history_smp_num/fs_hz, signal_duration_sec, 1/fs_hz, dtype=np.float64)


def generate_delayed_sin_matrix(*, smp_num, tone_freq_n, mag, history_smp_num=0, noise_level=0):
    assert np.abs(tone_freq_n) <= 1.0

    t = np.arange(-history_smp_num, smp_num, 1, dtype=np.float64)
    sig = mag * np.sin(2*np.pi * tone_freq_n * t, dtype=np.float64)
    sig += noise_level*np.random.randn(len(sig))

    # S is a matrix containing the delayed versions of 'sig': S[k,:] is 'sig' delayed by k taps
    S = np.lib.stride_tricks.sliding_window_view(sig, smp_num)
    S = np.flipud(S)

    return S


def generate_sin_signal(*, smp_num, tone_freq_n, mag, noise_level=0):
    S = generate_delayed_sin_matrix(smp_num=smp_num, tone_freq_n=tone_freq_n, mag=mag, noise_level=noise_level)
    return S[0,:]


def calc_spectrum(sig, fs=1):
    S = 20*np.log10(np.abs(fft.rfft(sig)))
    f = fft.rfftfreq(len(sig), d=1/fs)

    return f, S


def upsample_fft(signal, upsample_factor):
    fft_signal = fft.rfft(signal)

    zeros_to_insert = np.zeros((upsample_factor - 1) * len(fft_signal))
    fft_upsampled = np.concatenate((fft_signal, zeros_to_insert))
    upsampled_signal = fft.irfft(fft_upsampled, n=upsample_factor*len(signal)) * upsample_factor
    return upsampled_signal


def downsample_fft(signal, downsample_factor):
    fft_signal = fft.rfft(signal)
    fft_downsampled = fft_signal[:(len(fft_signal) // downsample_factor)]
    downsampled_signal = fft.irfft(fft_downsampled, n=len(signal)//downsample_factor) / downsample_factor
    return downsampled_signal


def resample(factor):
    """Decorator to upsample before and downsample after the function."""
    def decorator(func):
        def wrapper(signal, *args, **kwargs):
            upsampled_signal = upsample_fft(signal, factor)
            result = func(upsampled_signal, *args, **kwargs)
            downsampled_result = downsample_fft(result, factor)

            return downsampled_result
        return wrapper
    return decorator