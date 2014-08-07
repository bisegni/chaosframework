/**
 * 
 */
package it.infn.chaos.mds.secutiry;

import java.security.Key;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.SecureRandom;
import java.security.Security;

import sun.misc.BASE64Encoder;

/**
 * @author bisegni
 * 
 */
@SuppressWarnings("restriction")
public class RSAKeys {
	Key	pubKey	= null;
	Key	privKey	= null;

	public static void main(String[] args) throws NoSuchAlgorithmException, NoSuchProviderException {
		RSAKeys generateRSAKeys = new RSAKeys();
		generateRSAKeys.generate();
	}

	public void generate() throws NoSuchAlgorithmException, NoSuchProviderException {

		Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());

		// Create the public and private keys
		KeyPairGenerator generator = KeyPairGenerator.getInstance("RSA", "BC");
		SecureRandom random = createFixedRandom();
		generator.initialize(1024, random);

		KeyPair pair = generator.generateKeyPair();
		pubKey = pair.getPublic();
		privKey = pair.getPrivate();
	}

	public String getPublicKeyAsB64String() {
		if(pubKey == null) return null;
		BASE64Encoder b64 = new BASE64Encoder();
		return b64.encode(pubKey.getEncoded());
	}
	public String getPrivateKeyAsB64String() {
		if(privKey == null) return null;
		BASE64Encoder b64 = new BASE64Encoder();
		return b64.encode(privKey.getEncoded());
	}
	public byte[] getPublicKeyAsByte() {
		if(pubKey == null) return null;
		return pubKey.getEncoded();
	}
	
	public byte[] getPrivateKeyAsByte() {
		if(privKey == null) return null;
		return privKey.getEncoded();
	}
	
	public static SecureRandom createFixedRandom() {
		return new FixedRand();
	}

	private static class FixedRand extends SecureRandom {

		MessageDigest	sha;
		byte[]			state;

		FixedRand() {
			try {
				this.sha = MessageDigest.getInstance("SHA-1");
				this.state = sha.digest();
			} catch (NoSuchAlgorithmException e) {
				throw new RuntimeException("can't find SHA-1!");
			}
		}

		public void nextBytes(byte[] bytes) {

			int off = 0;

			sha.update(state);

			while (off < bytes.length) {
				state = sha.digest();

				if (bytes.length - off > state.length) {
					System.arraycopy(state, 0, bytes, off, state.length);
				} else {
					System.arraycopy(state, 0, bytes, off, bytes.length - off);
				}

				off += state.length;

				sha.update(state);
			}
		}
	}
}
