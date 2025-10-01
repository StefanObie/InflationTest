import requests
import os
import time
from datetime import datetime

# Configuration
CAMERAS = [
    {"name": "LEFT", "ip": "192.168.0.184", "folder": "img/left"},
    {"name": "RIGHT", "ip": "192.168.0.184", "folder": "img/right"}
]
INTERVAL = 1.0  # seconds between captures

def create_folder():
    """Create the img folder if it doesn't exist"""
    for camera in CAMERAS:
        if not os.path.exists(camera["folder"]):
            os.makedirs(camera["folder"])
            print(f"Created folder: {camera['folder']}")

def get_timestamp():
    """Generate timestamp for filename"""
    return datetime.now().strftime("%b%d_%Hh%Mm%Ss")

def save_image(camera):
    """Capture and save an image from the camera"""
    camera_name = camera["name"]
    camera_ip = camera["ip"]
    camera_folder = camera["folder"]
    capture_url = f"http://{camera_ip}/capture"
    
    try:
        # Send GET request to capture endpoint
        response = requests.get(capture_url, timeout=10)
        
        if response.status_code == 200:
            # Generate filename with timestamp
            timestamp = get_timestamp()
            filename = f"{camera_name}_{timestamp}.jpg"
            filepath = os.path.join(camera_folder, filename)
            
            # Save image
            with open(filepath, 'wb') as f:
                f.write(response.content)
            
            print(f"Saved: {filename} ({len(response.content)} bytes)")
            return True
        else:
            print(f"Failed to capture image. Status code: {response.status_code}")
            return False
            
    except requests.exceptions.RequestException as e:
        print(f"Error capturing image: {e}")
        return False

def main():
    """Main function to continuously capture images"""
    cameras_str = ", ".join([f"{c['name']} ({c['ip']})" for c in CAMERAS])
    print(f"Starting image capture for {cameras_str}")
    print(f"Interval: {INTERVAL} seconds")
    print("Press Ctrl+C to stop")
    
    create_folder()
    
    try:
        while True:
            for camera in CAMERAS:
                save_image(camera)
            time.sleep(INTERVAL)
            
    except KeyboardInterrupt:
        print("\nStopping image capture...")

if __name__ == "__main__":
    main()