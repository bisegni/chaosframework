/**
 * 
 */
package it.infn.chaos.mds;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStream;

import javax.management.Notification;

import com.vaadin.ui.Upload.Receiver;
import com.vaadin.ui.Upload.SucceededEvent;
import com.vaadin.ui.Upload.SucceededListener;

/**
 * @author Andrea Michelotti
 *
 */
class FileUploader implements Receiver {
    public File file;
    private String dstdir;
    public OutputStream receiveUpload(String filename,
                                      String mimeType) {
        // Create upload stream
        FileOutputStream fos = null; // Stream to write to
        
            
            // Open the file for writing.
            file = new File(dstdir +"/" + filename);
            
            try {
				fos = new FileOutputStream(file);
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
      
        return fos; // Return the output stream to write to
    }
    public FileUploader(String dst) {
 	   dstdir= dst; 
 	   File d = new File(dst);
 	   d.mkdirs();
 	   
    }
    

    
};