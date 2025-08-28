# watch_and_render.py
import time
import subprocess
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# TODO (stouff) make the file and scene configurable via command line
FILE_TO_WATCH = "derivation.py"  # your manim file
RENDER_CMD = ["manim", FILE_TO_WATCH, "DerivationGoal", "-ql", "--write_to_movie"]  # change class name and flags

class ReloadHandler(FileSystemEventHandler):
    def on_modified(self, event):
        if event.src_path.endswith(FILE_TO_WATCH):
            print(f"Detected change in {FILE_TO_WATCH}, rerendering...")
            subprocess.run(RENDER_CMD)

if __name__ == "__main__":
    event_handler = ReloadHandler()
    observer = Observer()
    observer.schedule(event_handler, path=".", recursive=False)
    observer.start()
    print("Watching for changes... Ctrl+C to stop.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
