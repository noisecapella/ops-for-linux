#include "ops-linux.h"

static int GetLength(FILE* fp) {
  int l;
  fseek(fp, 0, SEEK_END);
  l = ftell(fp);
  
  fseek(fp, 0, SEEK_SET);
  return l;
}

static gboolean UploadFile(char* saveto, char* filename) {
	// This function expects that you've already changed the parition and 
	// directory to the correct path in the camcorder. dirpath is the Windows
	// path to the file to store in.

	// directory.
  int filesize;
  unsigned int x;
  unsigned int count = 0, tcount = 0, alt_total = 0;
  //CFile file;
  FILE* file = NULL;
  char buffer [BUFSIZE];
  char sfilename[256];
  
  Log("entering UploadFile");
  printf("UploadFile(%s, %s)\n",saveto,filename);

  if(strlen(filename)>12) {
    Log("Can't upload files with long filenames");
    return(FALSE);
  }
  file = fopen(saveto, "rb");
  if (file == NULL) {
    Log("Trouble opening: ");
    Log(saveto);
    return FALSE;
  }
  filesize = GetLength(file);
  
  //  Log("proceeding in UploadFile...");

  memset(sfilename,0,255);
  strncpy((char *)sfilename,filename,12);
  if(ControlMessageWrite(0xb105, sfilename, strlen(sfilename)+1, TIMEOUT)==FALSE) { // SetFileName
    Log("failed at 0xb1");
    return FALSE;
  }

  // It's a bisexual byte order! Arrrgh! The PDP-11 legacy lives on.
  char udata[4];
  udata[1] = filesize >> 24;
  udata[0] = filesize >> 16;
  udata[3] = filesize >>  8;
  udata[2] = filesize >>  0;
  if(ControlMessageWrite(0x9505, udata, 4, TIMEOUT)== FALSE) { // Request Write
    Log("failed at 0x95");
    return FALSE;
  }

  x=0;
  //  EnableControls(FALSE);
  for(;;) {
    Log("writing...");
    count = fread(buffer, sizeof(char), BUFSIZE, file);
    if (count < 1) break;
    tcount += count;
    set_bitrate(tcount);
    if (tcount  - alt_total > 65536) {
      set_progress_bar((double)tcount / (double)filesize);
      //      fprintf(stderr,"m_progressbar: %f\n",m_progressbar_fraction);
      alt_total = tcount;
    }
    printf("tcount: %d count: %d\n", tcount, count);
    x=Write(buffer,count,TIMEOUT);
    //printf("x: %d\n",x);
    if(x<1) {
      Log("Error writing file to camera");
      fclose(file);
      //EnableControls(TRUE);
      //      m_progressbar_fraction = 0;
      set_progress_bar(0.0);
      return FALSE;
    }

    //Log("reading...");
    //if(count<BUFSIZE)
    //break;
    //x += count;
    //    printf("x: %d\n", x);
    //    file.WriteHuge(buffer,count);
    //    if(count<BUFSIZE)
    //break;
    //DoMessagePump();
  }

  //CString msg; msg.Format("file was %d bytes in size",x); Log(msg);
  // m_progressbar_fraction = 0;
  set_progress_bar(0.0);
  set_bitrate(0);
  fclose(file);
  //  Log("Success sending file");
  
  Log("Leaving UploadFile");
  return TRUE;
}


static void upload_file_start_thread(gpointer data) {
  threesome* ts = data; //see DownloadFile for args
  //EnableControls(FALSE);
  if(UploadFile((char*)(ts->a), (char*)(ts->b))==FALSE) {
    Log("UpFile(p->filename, tempstring) failed.");
  } else {
    Log("Success retrieving data file.");
  }
  free(ts->a);
  free(ts);
  set_progress_bar(0.0);
  EnableControls(TRUE);
}


gboolean upload_file( GtkWidget *widget,
		      GdkEvent *event,
		      gpointer data) {
  GtkWidget *file_selection_box = NULL;
  //  threesome ts;
  GError* error;
  //  GtkWidget* file_selection_box = NULL;
  
  if(CheckCameraOpen()==FALSE)
    return FALSE;
  
  //  using_dialog_mutex = TRUE;

  GtkWidget *file_selection_dialog = NULL;
  char* save_filename;
  //  GtkWidget* file_selection_box = NULL;
  
  // fill in name of file for easy clicking
  GtkTreeModel* model;
  GtkTreeIter iter;
  GtkTreeSelection* selection;
  char* temp_save_filename;
  //  gpointer data;
  file_info* currently_selected_file;
  //char tempfilename[STRINGSIZE];
  char delimiter;
  char saveto[STRINGSIZE];
  threesome* ts = NULL;
#ifdef _WIN32
  delimiter = '\\';
#else
  delimiter = '/';
#endif
  if(CheckCameraOpen()==FALSE)
    return FALSE;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_directory_tree));
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    //    gchar* filename = NULL;
    gtk_tree_model_get(model, &iter, COL_POINTER, &currently_selected_file, -1);
    if (currently_selected_file == NULL) {//not too likely
      return FALSE;
    }
  } else {
    return FALSE; //nothing selected for download
  }
  if (currently_selected_file->filetype != FIDIR && currently_selected_file->filetype != FIPART) {
    MessageBox("Only uploads to directories are currently supported.\nTry renaming your file and uploading it to a directory on the camcorder\nto overwrite another file on the camcorder.");
    return FALSE;
  }


  file_selection_dialog =
    gtk_file_chooser_dialog_new("Please select a file to upload",
				main_window, //meaingless except if program killed
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
  //  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(file_selection_dialog),currently_selected_file->filename );
  if (gtk_dialog_run(GTK_DIALOG(file_selection_dialog)) == GTK_RESPONSE_ACCEPT) {
    save_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selection_dialog));
    strncpy(saveto, save_filename, STRINGSIZE); //make things easier later on
  } else { //user cancelled
    gtk_widget_destroy(file_selection_dialog);
    return FALSE;
  }
  
  gtk_widget_destroy(file_selection_dialog); //does save_filename now point to garbage?



  
  if (currently_selected_file->filetype==FIROOT) {
    MessageBox("Sorry, uploading partitions is not supported");
    return FALSE;
  }
  
  if(ChangePartition(currently_selected_file->partition)==FALSE) {
    Log("ChangePartition(p->partition) failed.");
    return FALSE;
  }
  
  
  //if uploading to directory, make it use a filename you're saving to instead
  if (currently_selected_file->filetype == FIDIR || currently_selected_file->filetype == FIPART) {
    if(ChangeDirectory(currently_selected_file->fullpath) == FALSE) {
      Log("ERROR: ChangeDirectory to upload directory failed.");
      return FALSE;
    }
    temp_save_filename = saveto;
    while (*temp_save_filename++); //go to end
    while (--temp_save_filename != saveto) //back up to delimiter point
      if (*temp_save_filename == delimiter)
	break;
    ++temp_save_filename;
    if (strlen(temp_save_filename) > 12) {
      Log("Please rename this file to an 8.3 filename (ABCDEFGH.IJK for instance)");
      return FALSE;
    }
  } else {
    if(ChangeDirectory(currently_selected_file->dirpath) == FALSE) {
      Log("ERROR: ChangeDirectory to upload directory failed.");
      return FALSE;
    }

    temp_save_filename = currently_selected_file->filename;
  }
  ts = malloc(sizeof(threesome));
  if (ts == NULL) return FALSE;
  ts->a = malloc(sizeof(char) * (strlen(saveto) + 1));
  if (ts->a == NULL) { //extraordinarily rare...
    free(ts);
    return FALSE;
  }
  //temp_save_filename overlaps saveto, but this shouldn't cause trouble
  //  ts->b = temp_save_filename; //char*
  strcpy(ts->a, saveto); //char*
  if (currently_selected_file->filetype == FIDIR || currently_selected_file->filetype == FIPART) {
    ts->b = (temp_save_filename - saveto) + ts->a;
  } else {
    ts->b = currently_selected_file->filename + strlen(currently_selected_file->dirpath);
  }
  //  ts->c = &(currently_selected_file->filesize); //int
  EnableControls(FALSE);
  if (!g_thread_create(upload_file_start_thread, ts, FALSE, &error)) {
    Log(error->message);
    EnableControls(TRUE);
    return FALSE;
  }

  
  return TRUE;

  
}

