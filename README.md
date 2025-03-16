# fwd
Fwd is a simple and performant T.C.P. proxy swiss-army knife. The main purpose of fwd is to provide a simple "run anywhere" executable that can be spun up and deployed easily.

## Features
 - Full upstream and downstream T.L.S. security through OpenSSL.
 - Extremely simple codebase and minimal dependencies.
 - Application layer-agnosticity. Any application or service may be proxied through this application as long as said application or service supports proxying.
 - Full user control. If the underlying API's allow it, then fwd will allow it.
 - Easily dump traffic between a client and a service to a file or the terminal.

## Build

**Fedora Linux**
```bash
dnf install openssl-devel
make
```

## Terminology & Concepts
- The downstream server is the machine that fwd is running on.
- The upstream server is the service that fwd is proxying.
- The upstream server is proxied through the downstream server.

**1. T.L.S. (upstream) <---> T.L.S. (downstream) <---> T.L.S. (client)**
- The downstream server needs a set of two certificates and two keys.
- The downstream server needs one certificate and key pair to serve content over T.L.S. to the client.
- The downstream server also needs one certificate and key pair to retrieve content from the upstream server over T.L.S.
- This could be considered a "man-in-the-middle" configuration.

**2. Unencrypted T.C.P. (upstream) <---> T.L.S. (downstream) <---> T.L.S. (client)**
- The downstream server needs one certificate and key pair to serve content over T.L.S. to the client.
- The downstream server facilitates client requests to the upstream server over unencrypted T.C.P. but encrypts content between the proxy and the client.
- This could be considered a traditional reverse proxy configuration.

**3. T.L.S. (upstream) <---> Unencrypted T.C.P. (downstream) <---> Unencrypted T.C.P. (client)**
- The downstream server needs one certificate and key pair to retrieve content from the upstream server in an encrypted fashion.
- However, content remains unencrypted from the journey from the downstream server to the proxy.
- This could be considered as a proxy configuration.

**4. Unencrypted T.C.P. (upstream) <---> Unencrypted T.C.P. (downstream) <---> Unencrypted T.C.P. (client)**
- There is no encryption or message vertification between the upstream, downstream, and client.
- This could be considered as a masquerade configuration.

**Example Configurations**
- Create your own Certificate Authority and sign your own server certificates. Reroute D.N.S. to your downstream server and proxy it to whatever service you wish to emulate. You may then capture unencrypted traffic in a man-in-the-middle attack.
- Create a unauthenticated but encrypted and signed T.L.S. tunnel to protect your unencrypted traffic over the internet. On one machine, proxy an unencrypted TCP service as an encrypted TCP service over the internet. Then, on another machine proxy the other downstream service using a encrypted TLS to unencrypted TCP configuration. Route the unencrypted TCP configuration to your loopback address and then interact with the service using your loopback address.
- Bridge a service over two different networks together by running fwd on a machine connected to two different LANs together in an unencrypted upstream and downstream configuration.
