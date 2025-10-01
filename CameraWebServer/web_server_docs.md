# ESP32 Camera Web Server API Documentation

## Overview
This ESP32 camera web server provides REST API endpoints for capturing images, streaming video, and controlling camera settings. The server runs on two ports:
- **Main server**: Port 80 (default) - handles capture, control, and status endpoints
- **Stream server**: Port 81 - handles video streaming

## Base URL
```
http://192.168.0.184
```

## Endpoints

### 1. Main Interface
**GET /** 
- **Description**: Serves the main web interface
- **Response**: HTML page with camera controls (gzipped)
- **Content-Type**: `text/html`

### 2. Image Capture

#### JPEG Capture
**GET /capture**
- **Description**: Captures a single JPEG image
- **Response**: JPEG image data
- **Headers**: 
  - `Content-Type: image/jpeg`
  - `X-Timestamp: {timestamp}`
  - `Content-Disposition: inline; filename=capture.jpg`

#### BMP Capture  
**GET /bmp**
- **Description**: Captures a single BMP image
- **Response**: BMP image data
- **Headers**:
  - `Content-Type: image/x-windows-bmp` 
  - `X-Timestamp: {timestamp}`
  - `Content-Disposition: inline; filename=capture.bmp`

### 3. Video Streaming

#### MJPEG Stream
**GET /stream** (Port 81)
- **Description**: Live MJPEG video stream
- **Response**: Multipart MJPEG stream
- **Headers**:
  - `Content-Type: multipart/x-mixed-replace`
  - `X-Framerate: 60`

### 4. Camera Control

#### Set Camera Parameters
**GET /control?var={parameter}&val={value}**
- **Description**: Controls camera settings
- **Parameters**:

| Parameter | Description | Valid Values |
|-----------|-------------|--------------|
| `framesize` | Image resolution | 0-13 (0=96x96, 6=800x600, 10=1024x768, 13=1600x1200) |
| `quality` | JPEG quality | 4-63 (lower = better quality) |
| `brightness` | Brightness level | -2 to 2 |
| `contrast` | Contrast level | -2 to 2 |
| `saturation` | Saturation level | -2 to 2 |
| `gainceiling` | AGC gain ceiling | 0-6 |
| `colorbar` | Test pattern | 0=off, 1=on |
| `awb` | Auto white balance | 0=off, 1=on |
| `agc` | Auto gain control | 0=off, 1=on |
| `aec` | Auto exposure control | 0=off, 1=on |
| `hmirror` | Horizontal mirror | 0=off, 1=on |
| `vflip` | Vertical flip | 0=off, 1=on |
| `awb_gain` | AWB gain | 0=off, 1=on |
| `agc_gain` | AGC gain value | 0-30 |
| `aec_value` | AEC value | 0-1200 |
| `aec2` | AEC2 | 0=off, 1=on |
| `dcw` | DCW | 0=off, 1=on |
| `bpc` | BPC | 0=off, 1=on |
| `wpc` | WPC | 0=off, 1=on |
| `raw_gma` | Raw GMA | 0=off, 1=on |
| `lenc` | Lens correction | 0=off, 1=on |
| `special_effect` | Special effects | 0-6 (0=none, 1=negative, 2=grayscale, etc.) |
| `wb_mode` | White balance mode | 0-4 |
| `ae_level` | AE level | -2 to 2 |
| `led_intensity` | LED flash intensity | 0-255 (if LED available) |

**Example**: 
```
GET /control?var=framesize&val=10
GET /control?var=quality&val=12
GET /control?var=brightness&val=1
```

### 5. Status & Information

#### Get Camera Status
**GET /status**
- **Description**: Returns current camera settings and sensor registers
- **Response**: JSON object with all current settings
- **Content-Type**: `application/json`

**Example Response**:
```json
{
  "framesize": 6,
  "quality": 12,
  "brightness": 0,
  "contrast": 0,
  "saturation": 0,
  "awb": 1,
  "agc": 1,
  "aec": 1,
  "hmirror": 0,
  "vflip": 0,
  "led_intensity": 0
}
```

### 6. Advanced Controls

#### Set Clock Frequency
**GET /xclk?xclk={frequency}**
- **Description**: Sets camera clock frequency
- **Parameters**: 
  - `xclk`: Clock frequency in MHz (e.g., 10, 20)

#### Register Control
**GET /reg?reg={register}&mask={mask}&val={value}**
- **Description**: Sets camera sensor register
- **Parameters**:
  - `reg`: Register address (hex)
  - `mask`: Bit mask (hex) 
  - `val`: Value to set (hex)

#### Get Register Value
**GET /greg?reg={register}&mask={mask}**
- **Description**: Reads camera sensor register
- **Parameters**:
  - `reg`: Register address (hex)
  - `mask`: Bit mask (hex)
- **Response**: Register value as plain text

#### PLL Configuration
**GET /pll?bypass={0|1}&mul={value}&sys={value}&root={value}&pre={value}&seld5={0|1}&pclken={0|1}&pclk={value}**
- **Description**: Configure camera PLL settings
- **Parameters**: Various PLL configuration values

#### Resolution/Window Settings
**GET /resolution?sx={startX}&sy={startY}&ex={endX}&ey={endY}&offx={offsetX}&offy={offsetY}&tx={totalX}&ty={totalY}&ox={outputX}&oy={outputY}&scale={0|1}&binning={0|1}**
- **Description**: Configure camera sensor window/resolution settings
- **Parameters**: Window coordinates and scaling options

## Error Responses
- **404**: Endpoint not found or missing required parameters
- **500**: Camera error or operation failed

## Headers
All responses include:
- `Access-Control-Allow-Origin: *` (CORS enabled)

## Notes
- LED flash (if available) automatically activates during capture and streaming
- Stream endpoint runs on port 81 (main + 1)
- All endpoints support GET method only
- Register and PLL endpoints are for advanced users only