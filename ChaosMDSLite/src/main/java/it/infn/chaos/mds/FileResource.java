package it.infn.chaos.mds;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

import com.vaadin.terminal.StreamResource.StreamSource;

/*
 * @author Andrea Michelotti
 */
public class FileResource implements StreamSource {
		String fname;
		private static File lastResource=null;
		@Override
		public InputStream getStream() {
          
            FileInputStream helpFileInputStream=null;
			try {
				helpFileInputStream = new FileInputStream(fname);
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
            
			// TODO Auto-generated method stub
			return helpFileInputStream;
		}
		public FileResource(String f){fname = f;}
		
		public boolean removeLastResource(){
			
			File f = new File(fname);
			if(lastResource!=null){
				return lastResource.delete();
			}
			lastResource = f;
			return false;
		}

}
