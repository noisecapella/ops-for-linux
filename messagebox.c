#include "ops-linux.h"

static double_widget global_dw;

gboolean MessageBoxTextTwo (const char* st, gpointer data) {
  GtkWidget *window, 
    *ok_button,
    *cancel_button,
    *label,
    *hbox, 
    *textentry_a,
    *textentry_b,
    *vbox, *hbox_textentry;
  //  gpointer data = cc->function_pointer;
  //  char* textbox = malloc(STRINGSIZE); //up to user to clean this up
  window = gtk_dialog_new ();
  
  ok_button = gtk_button_new_with_label ("OK");
  cancel_button = gtk_button_new_with_label("Cancel");
  global_dw.a = gtk_entry_new_with_max_length(255); //just to be safe
  textentry_a = global_dw.a;
  global_dw.b = gtk_entry_new_with_max_length(255);
  textentry_b = global_dw.b;
  //  textbox = gtk_entry_get_text(textentry);
  //printf("textentry: %08x\n",textentry);
  gtk_signal_connect_object (GTK_OBJECT (cancel_button),
			     "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy), 
			     GTK_OBJECT(window));
  
  
  gtk_signal_connect_object (GTK_OBJECT (ok_button),
			     "clicked",
			     GTK_SIGNAL_FUNC (data),
			     &global_dw);
  
  gtk_signal_connect_object (GTK_OBJECT (ok_button),
			     "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy),
			     GTK_OBJECT(window));

  hbox = gtk_hbox_new(FALSE,0);
  hbox_textentry = gtk_hbox_new(FALSE,0);
  vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG(window)->action_area),
		      hbox, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_textentry),
		      textentry_a, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_textentry),
		      textentry_b, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox),
		      hbox_textentry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox),
		     ok_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox),
		     cancel_button, TRUE, TRUE, 0);
  
  /*gtk_widget_show(ok_button);
  gtk_widget_show(cancel_button);
  gtk_widget_show(hbox);
  gtk_widget_show(hbox_textentry);
  gtk_widget_show(vbox);
  gtk_widget_show(textentry);*/

  label = gtk_label_new(st);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, TRUE, TRUE, 0);
  gtk_widget_show_all(window);
  //for now ignore the button part, use window manager button on top

  
  return TRUE;
  
}


gboolean MessageBox(const char *st)
{
  GtkWidget *window, *ok_button, *label;
  Log("MessageBox: %s",st);
  window = gtk_dialog_new ();

  ok_button = gtk_button_new_with_label ("OK");
  gtk_signal_connect_object (GTK_OBJECT (ok_button),
			     "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy), 
			     GTK_OBJECT(window));
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area),
		     ok_button, TRUE, TRUE, 0);
  gtk_widget_show(ok_button);
  //for now ignore the button part, use window manager button on top
  label = gtk_label_new (st);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), label, TRUE, TRUE, 0);
  
  gtk_widget_show (label);
  gtk_widget_show(window);
  return TRUE;

}


char* MessageBoxText (const char* st) {
  GtkWidget *dialog, 
    *ok_button,
    *cancel_button,
    *label,
    *hbox, 
    *textentry,
    *vbox, *hbox_textentry;
  //  gpointer data = cc->function_pointer;
  //char* textbox = malloc(STRINGSIZE); //up to user to clean this up
  const char* textbox;
  char* returnvalue;
  gboolean result;
  dialog = gtk_dialog_new_with_buttons("Confirm",GTK_WINDOW(main_window),
                                       0,
                                       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                       NULL);
  
  textentry = gtk_entry_new_with_max_length(255); //just to be safe
  textbox = gtk_entry_get_text(GTK_ENTRY(textentry));
  returnvalue = (char*)malloc(strlen(textbox) + 1);
  strcpy(returnvalue, textbox);
  //printf("textentry: %08x\n",textentry);
  /*  gtk_signal_connect_object (GTK_OBJECT (cancel_button),
			     "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy), 
			     GTK_OBJECT(window));
  
  
  gtk_signal_connect_object (GTK_OBJECT (ok_button),
			     "clicked",
			     GTK_SIGNAL_FUNC (data),
			     textentry);
  
  gtk_signal_connect_object (GTK_OBJECT (ok_button),
			     "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy),
			     GTK_OBJECT(window));
  */

  hbox = gtk_hbox_new(FALSE,0);
  hbox_textentry = gtk_hbox_new(FALSE,0);
  vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG(dialog)->action_area),
		      hbox, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_textentry),
		      textentry, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox),
		      hbox_textentry, TRUE, TRUE, 0);
  
  /*gtk_widget_show(ok_button);
  gtk_widget_show(cancel_button);
  gtk_widget_show(hbox);
  gtk_widget_show(hbox_textentry);
  gtk_widget_show(vbox);
  gtk_widget_show(textentry);*/

  label = gtk_label_new(st);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), label, TRUE, TRUE, 0);
  gtk_widget_show_all(hbox);
  gtk_widget_show(label);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  if (result == GTK_RESPONSE_ACCEPT)
    return returnvalue;
  else {
    free(returnvalue);
    return NULL;
  }
}


gboolean MessageBoxConfirm (const char* st) {
  gboolean confirm;
  GtkWidget * dialog, * label;
  gint result;

  // assume "OK" if running in batch mode
  if (!main_window) return TRUE;

  // create dialog with ok/cancel buttons
  dialog = gtk_dialog_new_with_buttons("Confirm",GTK_WINDOW(main_window),
                                       0,
                                       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                       NULL);
  // add the box contents
  label = gtk_label_new (st);
  gtk_box_pack_start(GTK_BOX (GTK_DIALOG (dialog)->vbox), label, TRUE, TRUE, 0);
  gtk_widget_show (label);

  // run the dialog and check the result
  result = gtk_dialog_run( GTK_DIALOG(dialog) );
  switch (result) {
    case GTK_RESPONSE_ACCEPT:
      confirm = TRUE;
      break;
    default:
      confirm = FALSE;
      break;
  }

  // destroy the dialog
  gtk_widget_destroy(dialog);

  return confirm;
}

/*void Log (const char *st)
{
  fprintf (stderr, "%s\n", st);
  }*/
