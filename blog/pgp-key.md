## PGP

------

###### \[ [Ryan Bruno](https://rbruno.com/about) \] \[ [more](https://blog.rbruno.com) - [perma](https://blog.rbruno.com/p/pgp-key) \] \[ 05-21-2020 \]

My use of PGP provides you two pieces of mind.
First, the piece of mind that any post signed by my PGP key was written by the same person who wrote this document.
Second, the piece of mind that, should you decide to send me an email at ryan@rbruno.com, only I can read your email if you use PGP as well.

If you just want my public key [click here](#my-key).

### My use of PGP

A more in-depth post on PGP, GPG, and PKI is coming soon so lets go into how I am using PGP right now.
The HTML for all my webpages are clear signed.
A trick I found elsewhere using HTML comments allows this.
The page follows the template below:

```
<!DOCTYPE html>
<html>
<!--
 -----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

 -->
<head>
...
</body>
<!--
 -----BEGIN PGP SIGNATURE-----

...
 -----END PGP SIGNATURE-----
 -->
</html>
```

Given this we can build every thing of our page except the opening and closing HTML tag, begin and end the document with dangling comment tags, then clear sign that entire document.
Even after adding the HTML tags before and after the signature blocks upon downloading the page and verifying the signature you should see something like (in GPG):

```
gpg: Signature made Thu 21 May 2020 06:40:09 PM EDT
gpg:                using RSA key 209AE7CC2F011A57D74D3B1BF81ED637A042A2BE
gpg: Good signature from "Ryan Bruno <ryan@rbruno.com>" [ultimate]
```

This provides you the piece of mind that the author of this page has not been taken over by some malicious man-in-the-middle trying the spread false Linux tips.

#### PGP Encryption

As a note, if you would like to send me emails please encrypt them to my PGP encryption key.

### Possible Pitfalls

Off the top of my head I can think of a few areas that may be subject for attacks in this system.
One is with the clear signed HTML method above.
Due to the fact that the entire HTML tag is not signed an attacker could inject scripts before or after this the signed text.
If you see any code or markup other then the HTML tag outside the signed text then you can be sure something fishy is going on.
A major attack area would be an impostor key. 
Time and TLS are the only defences in this situation.
This page should be protected by a TLS certificate that has been signed by a CA so downloading my key from here should be fairly trusted.
TLS is not perfect so in the case of a rouge CA or stolen key you can only be certain my key is trusted if you have the key in your keyring and trusted it before the attack.
The Web Of Trust could apply if you have friends who have, in the past, signed my key or send you the correct one directly.

## My Key

Key Server Link: TBD

Fingerprint: 6A02 2A8C 5245 6490 86F0  EC6D 763C 9E1D EE16 B59D

Public Key:

```
-----BEGIN PGP PUBLIC KEY BLOCK-----

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
L6uVJlj5ZWMW9L2FdO+HlYIZGA==
=L2cY
-----END PGP PUBLIC KEY BLOCK-----
```
