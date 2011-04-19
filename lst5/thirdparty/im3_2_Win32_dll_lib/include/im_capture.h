/** \file
 * \brief Video Capture
 *
 * See Copyright Notice in im.h
 * $Id: im_capture.h,v 1.8 2006/11/20 13:18:51 scuri Exp $
 */

#ifndef __IM_CAPTURE_H
#define __IM_CAPTURE_H

#if     defined(__cplusplus)
extern "C" {
#endif

/* declarations to create an export library for Watcom. */
#if ! defined (IM_DECL)   
  #if defined (__WATCOMC__)
    #define IM_DECL   __cdecl
  #elif defined(__WATCOM_CPLUSPLUS__)
    #define IM_DECL   __cdecl
  #else
    #define IM_DECL
  #endif
#endif

/** \defgroup capture Image Capture 
 * \par
 * Functions to capture images from live video devices.
 * \par
 * See \ref im_capture.h
 */

typedef struct _imVideoCapture imVideoCapture;

/** Returns the number of available devices.
 *
 * \verbatim im.VideoCaptureDeviceCount() -> count: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureDeviceCount(void);

/** Returns the device description. Returns NULL only if it is an invalid device.
 *
 * \verbatim im.VideoCaptureDeviceDesc(device: number) -> desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
const char* IM_DECL imVideoCaptureDeviceDesc(int device);

/** Returns the extendend device description. May return NULL.
 *
 * \verbatim im.VideoCaptureDeviceExDesc(device: number) -> desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
const char* imVideoCaptureDeviceExDesc(int device);

/** Returns the device path configuration. This is a unique string. 
 *
 * \verbatim im.VideoCaptureDevicePath(device: number) -> desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
const char* imVideoCaptureDevicePath(int device);

/** Returns the vendor information. May return NULL.
 *
 * \verbatim im.VideoCaptureDeviceVendorInfo(device: number) -> desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
const char* imVideoCaptureDeviceVendorInfo(int device);

/** Reload the device list. The devices can be dynamically removed or added to the system.
 * Returns the number of available devices.
 *
 * \verbatim im.imVideoCaptureReloadDevices() -> count: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureReloadDevices(void);

/** Creates a new imVideoCapture object. \n
 * Returns NULL if there is no capture device available. \n
 * In Windows returns NULL if DirectX version is older than 8.
 *
 * \verbatim im.VideoCaptureCreate() -> vc: imVideoCapture [in Lua 5] \endverbatim
 * \ingroup capture */
imVideoCapture* IM_DECL imVideoCaptureCreate(void);

/** Destroys a imVideoCapture object.
 *
 * \verbatim im.VideoCaptureDestroy(vc: imVideoCapture) [in Lua 5] \endverbatim
 * \verbatim vc:Destroy() [in Lua 5] \endverbatim
 * \ingroup capture */
void IM_DECL imVideoCaptureDestroy(imVideoCapture* vc);

/** Connects to a capture device. 
 * More than one imVideoCapture object can be created
 * but they must be connected to different devices.  \n
 * If the object is conected it will disconnect first. \n
 * Use -1 to return the current connected device, 
 * in this case returns -1 if not connected. \n
 * Returns zero if failed.
 *
 * \verbatim vc:Connect([device: number]) -> ret: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureConnect(imVideoCapture* vc, int device);

/** Disconnect from a capture device.
 *
 * \verbatim vc:Disconnect() [in Lua 5] \endverbatim
 * \ingroup capture */
void IM_DECL imVideoCaptureDisconnect(imVideoCapture* vc);

/** Returns the number of available configuration dialogs.
 *
 * \verbatim vc:DialogCount() -> count: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureDialogCount(imVideoCapture* vc);

/** Displays a configuration modal dialog of the connected device. \n
 * In Windows, the capturing will be stopped in some cases. \n
 * In Windows parent is a HWND of a parent window, it can be NULL. \n
 * dialog can be from 0 to \ref imVideoCaptureDialogCount. \n
 * Returns zero if failed.
 *
 * \verbatim vc:ShowDialog(dialog: number, parent: userdata) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureShowDialog(imVideoCapture* vc, int dialog, void* parent);

/** Allows to control the input and output of devices that have multiple input and outputs.
 * cross controls in which stage the input/output will be set. Usually use 1, but some capture boards
 * has a second stage. In Direct X it controls the crossbars.
 *
 * \verbatim vc:SetInOut(input, output, cross: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int imVideoCaptureSetInOut(imVideoCapture* vc, int input, int output, int cross);

/** Returns the description of a configuration dialog.
 * dialog can be from 0 to \ref imVideoCaptureDialogCount. \n
 *
 * \verbatim vc:DialogDesc(dialog: number) -> desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
const char* IM_DECL imVideoCaptureDialogDesc(imVideoCapture* vc, int dialog);

/** Returns the number of available video formats. \n
 * Returns zero if failed.
 *
 * \verbatim vc:FormatCount() -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureFormatCount(imVideoCapture* vc);

/** Returns information about the video format. \n
 * format can be from 0 to \ref imVideoCaptureFormatCount. \n
 * desc should be of size 10. \n
 * The image size is usually the maximum size for that format. 
 * Other sizes can be available using \ref imVideoCaptureSetImageSize. \n
 * Returns zero if failed.
 *
 * \verbatim vc:GetFormat(format: number) -> error: number, width: number, height: number, desc: string [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureGetFormat(imVideoCapture* vc, int format, int *width, int *height, char* desc);

/** Changes the video format of the connected device. \n 
 * Should NOT work for DV devices. Use \ref imVideoCaptureSetImageSize only. \n
 * Use -1 to return the current format, in this case returns -1 if failed. \n
 * When the format is changed in the dialog, for some formats 
 * the returned format is the preferred format, not the current format. \n
 * This will not affect color_mode of the capture image. \n
 * Returns zero if failed.
 *
 * \verbatim vc:SetFormat(format: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureSetFormat(imVideoCapture* vc, int format);

/** Returns the current image size of the connected device. \n
 *  width and height returns 0 if not connected.
 *
 * \verbatim vc:GetImageSize() -> width: number, height: number [in Lua 5] \endverbatim
 * \ingroup capture */
void IM_DECL imVideoCaptureGetImageSize(imVideoCapture* vc, int *width, int *height);

/** Changes the image size of the connected device. \n 
 * Similar to \ref imVideoCaptureSetFormat, but changes only the size. \n
 * Valid sizes can be obtained with  \ref imVideoCaptureGetFormat. \n
 * Returns zero if failed.
 *
 * \verbatim vc:SetImageSize(width: number, height: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureSetImageSize(imVideoCapture* vc, int width, int height);

/** Returns a new captured frame. Use -1 for infinite timeout. \n
 * Color space can be IM_RGB or IM_GRAY, and mode can be packed (IM_PACKED) or not. \n
 * It can not have an alpha channel and orientation is always bottom up. \n
 * Returns zero if failed or timeout expired, the buffer is not changed.
 *
 * \verbatim vc:Frame(image: imImage, timeout: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureFrame(imVideoCapture* vc, unsigned char* data, int color_mode, int timeout);

/** Start capturing, returns the new captured frame and stop capturing.  \n
 * This is more usefull if you are switching between devices. \n
 * Data format is the same as imVideoCaptureFrame. \n
 * Returns zero if failed.
 *
 * \verbatim vc:OneFrame(image: imImage) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureOneFrame(imVideoCapture* vc, unsigned char* data, int color_mode);

/** Start capturing. \n
 * Use -1 to return the current state. \n
 * Returns zero if failed.
 *
 * \verbatim vc:Live(live: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureLive(imVideoCapture* vc, int live); 

/** Resets a camera or video attribute to the default value or 
 * to the automatic setting. \n
 * Not all attributes support automatic modes. \n
 * Returns zero if failed.
 *
 * \verbatim vc:ResetAttribute(attrib: string, fauto: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureResetAttribute(imVideoCapture* vc, const char* attrib, int fauto);

/** Returns a camera or video attribute in percentage of the valid range value. \n
 * Returns zero if failed or attribute not supported.
 *
 * \verbatim vc:GetAttribute(attrib: string) -> error: number, percent: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureGetAttribute(imVideoCapture* vc, const char* attrib, float *percent);

/** Changes a camera or video attribute in percentage of the valid range value. \n
 * Returns zero if failed or attribute not supported.
 *
 * \verbatim vc:SetAttribute(attrib: string, percent: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup capture */
int IM_DECL imVideoCaptureSetAttribute(imVideoCapture* vc, const char* attrib, float percent);

/** Returns a list of the description of the valid attributes for the device class. \n
 * But each device may still not support some of the returned attributes. \n
 * Use the return value of \ref imVideoCaptureGetAttribute to check if the attribute is supported.
 *
 * \verbatim vc:GetAttributeList() -> attrib_list: table of strings [in Lua 5] \endverbatim
 * \ingroup capture */
const char** IM_DECL imVideoCaptureGetAttributeList(imVideoCapture* vc, int *num_attrib);


/** \defgroup winattrib Windows Attributes Names
 * Not all attributes are supported by each device.
 * Use the return value of \ref imVideoCaptureGetAttribute to check if the attribute is supported.
\verbatim
  VideoBrightness - Specifies the brightness, also called the black level. 
  VideoContrast - Specifies the contrast, expressed as gain factor. 
  VideoHue - Specifies the hue angle. 
  VideoSaturation - Specifies the saturation.
  VideoSharpness - Specifies the sharpness. 
  VideoGamma - Specifies the gamma. 
  VideoColorEnable - Specifies the color enable setting. (0/100)
  VideoWhiteBalance - Specifies the white balance, as a color temperature in degrees Kelvin. 
  VideoBacklightCompensation - Specifies the backlight compensation setting. (0/100)
  VideoGain - Specifies the gain adjustment.
  CameraPanAngle - Specifies the camera's pan angle. To 100 rotate right, To 0 rotate left (view from above).
  CameraTiltAngle - Specifies the camera's tilt angle.  To 100 rotate up, To 0 rotate down.
  CameraRollAngle - Specifies the camera's roll angle. To 100 rotate right, To 0 rotate left.
  CameraLensZoom - Specifies the camera's zoom setting. 
  CameraExposure - Specifies the exposure setting. 
  CameraIris - Specifies the camera's iris setting. 
  CameraFocus - Specifies the camera's focus setting, as the distance to the optimally focused target. 
  FlipHorizontal - Specifies the video will be flipped in the horizontal direction.
  FlipVertical - Specifies the video will be flipped in the vertical direction.
  AnalogFormat - Specifies the video format standard NTSC, PAL, etc. Valid values:
      NTSC_M     = 0 
      NTSC_M_J   = 1  
      NTSC_433   = 2
      PAL_B      = 3
      PAL_D      = 4
      PAL_H      = 5
      PAL_I      = 6
      PAL_M      = 7
      PAL_N      = 8
      PAL_60     = 9
      SECAM_B    = 10
      SECAM_D    = 11
      SECAM_G    = 12
      SECAM_H    = 13
      SECAM_K    = 14
      SECAM_K1   = 15
      SECAM_L    = 16
      SECAM_L1   = 17
      PAL_N_COMBO = 18
\endverbatim
 * \ingroup capture */


#if defined(__cplusplus)
}

/** A C++ Wrapper for the imVideoCapture structure functions.
 * \ingroup capture */
class imCapture
{
public:
  imCapture() 
    { vc = imVideoCaptureCreate(); }
  
  ~imCapture() 
    { if (vc) imVideoCaptureDestroy(vc); }
  
  int Failed() 
    { if (!vc) return 0; else return 1; }
  
  int Connect(int device) 
    { return imVideoCaptureConnect(vc, device); }
  
  void Disconnect() 
    { imVideoCaptureDisconnect(vc); }
  
  int DialogCount() 
    { return imVideoCaptureDialogCount(vc); }
  
  int ShowDialog(int dialog, void* parent) 
    { return imVideoCaptureShowDialog(vc, dialog, parent); }
  
  const char* DialogDescription(int dialog) 
    { return imVideoCaptureDialogDesc(vc, dialog); }

  int FormatCount()
    { return imVideoCaptureFormatCount(vc); }

  int GetFormat(int format, int *width, int *height, char* desc)
    { return imVideoCaptureGetFormat(vc, format, width, height, desc); }

  int SetFormat(int format)
    { return imVideoCaptureSetFormat(vc, format); }
  
  void GetImageSize(int *width, int *height) 
    { imVideoCaptureGetImageSize(vc, width, height); }
  
  int SetImageSize(int width, int height) 
    { return imVideoCaptureSetImageSize(vc, width, height); }
  
  int GetFrame(unsigned char* data, int color_mode, int timeout) 
    { return imVideoCaptureFrame(vc, data, color_mode, timeout); }
  
  int GetOneFrame(unsigned char* data, int color_mode) 
    { return imVideoCaptureOneFrame(vc, data, color_mode); }
  
  int Live(int live) 
    { return imVideoCaptureLive(vc, live); }
  
  int ResetAttribute(const char* attrib, int fauto) 
    { return imVideoCaptureResetAttribute(vc, attrib, fauto); }
  
  int GetAttribute(const char* attrib, float *percent) 
    { return imVideoCaptureGetAttribute(vc, attrib, percent); }
  
  int SetAttribute(const char* attrib, float percent) 
    { return imVideoCaptureSetAttribute(vc, attrib, percent); }
  
  const char** GetAttributeList(int *num_attrib) 
    { return imVideoCaptureGetAttributeList(vc, num_attrib); }
  
protected:
  imVideoCapture* vc;
};

#endif

#endif
