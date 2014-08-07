/**
 * 
 */
package it.infn.chaos.mds.secutiry;

import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;

/**
 * @author bisegni
 *
 */
public class Test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		RSAKeys ks = new RSAKeys();
		try {
			String pubkeyStr = null;
			String privkeyStr = null;
			ks.generate();
			RSAKey pubk = new RSAKey();
			RSAKey privk = new RSAKey();
			
			System.out.println(pubkeyStr = ks.getPublicKeyAsB64String());
			System.out.println(privkeyStr = ks.getPrivateKeyAsB64String());
			
			pubk.loadBase64Key(pubkeyStr, true);
			privk.loadBase64Key(privkeyStr, false);

			
			byte[] encodedString = pubk.encriptByte("che bella casa".getBytes());
			System.out.println(RSAKey.byteArrayToHexString(encodedString));
			
			byte[] decodeString = privk.decriptByte(RSAKey.hexStringToByteArray(RSAKey.byteArrayToHexString(encodedString)));
			System.out.println(new String(decodeString));
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
