#include "ops-linux.h"

gboolean enable_mass_storage(GtkWidget* widget,
			     GdkEvent* event,
			     gpointer data) {
  if (CheckCameraOpen() == FALSE)
    return FALSE;
  
  if (Monitor("wl 8013dee8 0xa") == TRUE) {
    Log("Sent Mass Storage enable command");
    MessageBox("Press 'PLAYBACK' on the camcorder to enable Mass Storage mode.\n\nNote: You will need to power-cycle the cam and re-open it to restore\nnormal communications with Ops for linux");
    return TRUE;
  } else {
    Log("Failed to send Mass Storage Mode enable command");
  }
  return FALSE;
  
}