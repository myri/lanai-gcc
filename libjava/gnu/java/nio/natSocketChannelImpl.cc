// natSelectorImpl.cc

/* Copyright (C) 2002  Free Software Foundation

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

#include <config.h>
#include <platform.h>

#include <errno.h>

#include <gcj/cni.h>
#include <gnu/java/nio/SocketChannelImpl.h>
#include <java/io/IOException.h>
#include <java/net/InetAddress.h>
#include <java/net/SocketException.h>

jint
gnu::java::nio::SocketChannelImpl::SocketCreate ()
{
  int sock = _Jv_socket (AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return sock;
}

jint
gnu::java::nio::SocketChannelImpl::SocketConnect (jint fd,
                                                  ::java::net::InetAddress *addr,
                                                  jint port)
{
  int result = _Jv_connect_address (fd, addr, port, addr, port);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}

jint
gnu::java::nio::SocketChannelImpl::SocketBind (jint fd,
                                               ::java::net::InetAddress *addr,
                                               jint port)
{
  int result = _Jv_bind_address (fd, addr, port);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}

jint
gnu::java::nio::SocketChannelImpl::SocketListen (jint fd, jint backlog)
{
  int result = _Jv_listen (fd, backlog);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}

jint
gnu::java::nio::SocketChannelImpl::SocketAvailable (jint /*fd*/)
{
  throw new ::java::net::SocketException (JvNewStringLatin1 ("SocketAvailable: not implemented"));
}

jint
gnu::java::nio::SocketChannelImpl::SocketClose (jint fd)
{
  int result = _Jv_close (fd);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}

jint
gnu::java::nio::SocketChannelImpl::SocketRead (jint fd, jbyteArray data,
                                               jint offset, jint length)
{
  int result = ::recv (fd, data, offset, length);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}

jint
gnu::java::nio::SocketChannelImpl::SocketWrite (jint fd, jbyteArray data,
                                                jint offset, jint length)
{
  int result = ::send (fd, data, offset, length);

  if (result < 0)
    {
      char* strerr = strerror (errno);
      throw new ::java::io::IOException (JvNewStringUTF (strerr));
    }

  return result;
}