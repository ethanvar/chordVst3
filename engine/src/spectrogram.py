import soundfile as sf
from pathlib import Path
import librosa
import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf


# class Spectrogram:
#     def __init__(self):
#         super(Spectrogram, self).__init__()

#     def forward(self, x):
#         # Implement spectrogram calculation here
#         return x

def plot_spectrogram_and_save(signal, sample_rate: float, spec_one_path: Path, spec_two_path: Path, min_freq: float, max_freq: float):
    stft = librosa.stft(signal)
    print("stft shape: ", stft.shape)
    print(stft[1, 1])
    spectrogram = np.abs(stft)
    spectrogram_db = librosa.amplitude_to_db(spectrogram)
    


    plt.figure(figsize=(25,10))
    img = librosa.display.specshow(spectrogram_db, y_axis='log', x_axis='time', sr=sample_rate, cmap='plasma')
    plt.savefig(spec_one_path)

    frequencies = librosa.fft_frequencies(sr=sample_rate)

    freq_one_high = float(input("Enter maximm frequency (Hz):  "))
    freq_one_low = float(input("Enter minimum frequency (Hz):  "))
    freq_one_mask = (frequencies > freq_one_low) & (frequencies < freq_one_high) 

    freq_two_high = float(input("Enter maximum frequency (Hz):  "))
    freq_two_low = float(input("Enter minimum frequency (Hz):  "))
    freq_two_mask = (frequencies > freq_two_low) & (frequencies < freq_two_high) 

    freq_three_high = float(input("Enter maximum frequency (Hz):  "))
    freq_three_low = float(input("Enter minimum frequency (Hz):  "))
    freq_three_mask = (frequencies > freq_three_low) & (frequencies < freq_three_high) 

    signal_time_marker = stft.shape[1]



    # Apply user-defined frequency mask
    # freq_mask = (frequencies > min_freq) & (frequencies < max_freq)
    masked_stft = np.zeros_like(stft)
    masked_stft[freq_one_mask, 0:signal_time_marker] = stft[freq_one_mask, 0:signal_time_marker]
    masked_stft[freq_two_mask, signal_time_marker:signal_time_marker*2] = stft[freq_two_mask, signal_time_marker:signal_time_marker*2]
    masked_stft[freq_three_mask, signal_time_marker*2:] = stft[freq_three_mask, signal_time_marker*2:]



    # For visualization, use the masked spectrogram
    masked_spectrogram = np.abs(masked_stft)
    masked_spectrogram_db = librosa.amplitude_to_db(masked_spectrogram)

    # Reconstruct the signal from the masked STFT
    reconstructed_signal = librosa.istft(masked_stft)

    # Save the reconstructed audio
    sf.write(f'../../songs/output.wav', reconstructed_signal, sample_rate)

    print(f"Sample rate: {sample_rate}")
    print(f"Filtered frequencies: {min_freq} Hz to {max_freq} Hz")

    plt.figure(figsize=(25,10))
    img = librosa.display.specshow(masked_spectrogram_db, y_axis='log', x_axis='time', sr=sample_rate, cmap='plasma')
    plt.savefig(spec_two_path)

    return masked_spectrogram_db

def sift_through_spectrogram(): 
    
    return



def select_song_block():
    song1 = 'LUCKI_-_NO_BAP_(Official_Video).mp3'
    song2 = 'bladee_-_Reality_Surf_(video).mp3'
    song3 = 'Luciano_Pavarotti_sings_"Nessun_dorma"_from_Turandot_(The_Three_Tenors_in_Concert_1994).mp3'
    spectrogram = []
    user_input = "start"

    # Ask user for frequency range
    while True:
        try:
            min_freq = float(input("Enter minimum frequency (Hz): "))
            max_freq = float(input("Enter maximum frequency (Hz): "))
            if min_freq >= max_freq or min_freq < 0:
                print("Invalid frequency range. Try again.")
                continue
            break
        except ValueError:
            print("Please enter valid numbers for frequencies.")

    user_input = input("Enter song number: ")
    spec_one_name = input("Enter the default spectrogram name: ")
    spec_two_name = input("Enter the edited spectrogram name: ")

    match user_input:
        case "1":
            signal, samplerate = librosa.load(Path(f'../../songs/{song1}'))
            spectrogram = plot_spectrogram_and_save(signal, samplerate, Path('spectrograms') / f'{spec_one_name}.jpeg', Path('spectrograms') / f'{spec_two_name}.jpeg', min_freq, max_freq)
        case "2":
            signal, samplerate = librosa.load(Path(f'../../songs/{song2}'))
            spectrogram = plot_spectrogram_and_save(signal, samplerate, Path('spectrograms') / f'{spec_one_name}.jpeg', Path('spectrograms') / f'{spec_two_name}.jpeg', min_freq, max_freq)
        case "3":
            signal, samplerate = librosa.load(Path(f'../../songs/{song3}'))
            spectrogram = plot_spectrogram_and_save(signal, samplerate, Path('spectrograms') / f'{spec_one_name}.jpeg', Path('spectrograms') / f'{spec_two_name}.jpeg', min_freq, max_freq)
        case _:
            print("No indexed at this number")
    
    return spectrogram

    

def main():
    # song1 = 'Frost_Children_-_Falling_(Official_Video).mp3'
    # song2 = 'Milk.mp3'
    # song3 = 'Bound2U.mp3'

    print("Type 'next' to go next and 'bye' to end program")

    spectrogram = select_song_block()
    
    user_input = ""
    while user_input != "bye":
        user_input = input("Display n array: ")

        match user_input:
            case "yes":
                print(spectrogram)
            case "no":
                print("Ending program.")
                break
            case _:
                print(len(spectrogram))
        
    

if __name__ == "__main__":
    main()