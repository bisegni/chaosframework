/**
 * 
 */
package it.infn.chaos.mds.secutiry;

import java.io.IOException;
import java.security.Security;

import org.bouncycastle.crypto.AsymmetricBlockCipher;
import org.bouncycastle.crypto.InvalidCipherTextException;
import org.bouncycastle.crypto.engines.RSAEngine;
import org.bouncycastle.crypto.params.AsymmetricKeyParameter;
import org.bouncycastle.crypto.params.RSAKeyParameters;
import org.bouncycastle.crypto.util.PrivateKeyFactory;
import org.bouncycastle.crypto.util.PublicKeyFactory;
import org.bouncycastle.util.encoders.Base64Encoder;

import sun.misc.BASE64Encoder;
import sun.misc.BASE64Decoder;

/**
 * @author bisegni
 * 
 */
@SuppressWarnings("restriction")
public class RSAKey {

	protected AsymmetricKeyParameter	key			= null;
	protected AsymmetricBlockCipher		encriptor	= null;
	protected AsymmetricBlockCipher		decriptor	= null;
	
	public RSAKey(){
		
	}
	
	public RSAKey(String base64Encoded, boolean isPublic) throws IOException {
		loadBase64Key(base64Encoded, isPublic);
	}
	
	public RSAKey(byte[] keyData, boolean isPublic) throws IOException {
		loadKey(keyData, isPublic);
	}
	public void loadBase64Key(String keyData, boolean isPublic) throws IOException {
		BASE64Decoder b64 = new BASE64Decoder();
		loadKey(b64.decodeBuffer(keyData), isPublic);
	}

	public void loadKey(byte[] keyData, boolean isPublic) throws IOException {
		Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
		key = isPublic ? PublicKeyFactory.createKey(keyData) : PrivateKeyFactory.createKey(keyData);
		encriptor = new org.bouncycastle.crypto.encodings.PKCS1Encoding(new RSAEngine());
		encriptor.init(true, key);
		decriptor = new org.bouncycastle.crypto.encodings.PKCS1Encoding(new RSAEngine());
		decriptor.init(false, key);
	}
	
	public byte[] encriptByte(byte[] data) throws InvalidCipherTextException {
		return encriptor.processBlock(data, 0, data.length);
	}

	public byte[] encriptString(String data) throws InvalidCipherTextException {
		byte[] messageBytes = data.getBytes();
		return encriptor.processBlock(messageBytes, 0, messageBytes.length);
	}

	public byte[] encriptHexString(String hexString) throws InvalidCipherTextException {
		byte[] messageBytes = hexStringToByteArray(hexString);
		return encriptor.processBlock(messageBytes, 0, messageBytes.length);
	}

	public byte[] decriptByte(byte[] data) throws InvalidCipherTextException {
		return decriptor.processBlock(data, 0, data.length);
	}

	public byte[] decriptString(String data) throws InvalidCipherTextException {
		byte[] messageBytes = data.getBytes();
		return decriptor.processBlock(messageBytes, 0, messageBytes.length);
	}

	public byte[] decriptHexString(String hexString) throws InvalidCipherTextException {
		byte[] messageBytes = hexStringToByteArray(hexString);
		return decriptor.processBlock(messageBytes, 0, messageBytes.length);
	}

	public String toBase64String(){
		BASE64Encoder b64 = new BASE64Encoder();
		return b64.encode(((RSAKeyParameters)key).getModulus().toByteArray());
	}
	
	public static String byteArrayToHexString(byte[] b) throws Exception {
		String result = "";
		for (int i = 0; i < b.length; i++) {
			result += Integer.toString((b[i] & 0xff) + 0x100, 16).substring(1);
		}
		return result;
	}

	public static byte[] hexStringToByteArray(String s) {
		int len = s.length();
		byte[] data = new byte[len / 2];
		for (int i = 0; i < len; i += 2) {
			data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character.digit(s.charAt(i + 1), 16));
		}
		return data;
	}
}
