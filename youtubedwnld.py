from pytube import YouTube
from pydub import AudioSegment
import os

def download_youtube_audio_as_mp3(url):
    try:
        # Set up custom headers (optional fix)
        yt = YouTube(url, use_oauth=False, allow_oauth_cache=True)
        print(f"Downloading: {yt.title}")

        audio_stream = yt.streams.filter(only_audio=True).order_by('abr').desc().first()
        if not audio_stream:
            print("No audio stream found.")
            return

        output_path = audio_stream.download(filename="temp_audio")
        print(f"Downloaded to: {output_path}")

        mp3_filename = yt.title.replace(" ", "_").replace("/", "_") + ".mp3"
        mp3_path = os.path.join(os.getcwd(), mp3_filename)

        print("Converting to MP3...")
        audio = AudioSegment.from_file(output_path)
        audio.export(mp3_path, format="mp3")

        os.remove(output_path)
        print(f"Saved MP3 as: {mp3_path}")

    except Exception as e:
        print("Error:", str(e))

if __name__ == "__main__":
    url = input("Enter the YouTube video URL: ").strip()
    download_youtube_audio_as_mp3(url)