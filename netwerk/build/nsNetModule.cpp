/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsCOMPtr.h"
#include "nsIModule.h"
#include "nsIGenericFactory.h"
#include "nsIComponentManager.h"
#include "nsIServiceManager.h"
#include "nsICategoryManager.h"
#include "nsNetModuleMgr.h"
#include "nsSocketProviderService.h"
#include "nscore.h"
#include "nsSimpleURI.h"
#include "nsDnsService.h"
#include "nsLoadGroup.h"
#include "nsStreamLoader.h"
#include "nsUnicharStreamLoader.h"
#include "nsDownloader.h"
#include "nsAsyncStreamListener.h"
#include "nsFileStreams.h"
#include "nsBufferedStreams.h"
#include "nsMIMEInputStream.h"
#include "nsProtocolProxyService.h"
#include "nsSOCKSSocketProvider.h"
#include "nsSOCKS4SocketProvider.h"
#include "nsCacheService.h"

#include "nsNetCID.h"

#if defined(XP_MAC) || defined(XP_MACOSX)
// Mac OS
#define BUILD_APPLEFILE_DECODER 1
#else
// other platforms
#define BUILD_BINHEX_DECODER 1
#endif

///////////////////////////////////////////////////////////////////////////////

#include "nsIOService.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsIOService, Init)

#include "nsStreamTransportService.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsStreamTransportService, Init)

#include "nsSocketTransportService2.h"
#undef LOG
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsSocketTransportService, Init)

#include "nsAsyncStreamCopier.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsAsyncStreamCopier)

#include "nsInputStreamPump.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsInputStreamPump)

#include "nsInputStreamChannel.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsInputStreamChannel)

///////////////////////////////////////////////////////////////////////////////

#include "nsStreamConverterService.h"

#ifdef BUILD_APPLEFILE_DECODER
#include "nsAppleFileDecoder.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsAppleFileDecoder)
#endif

///////////////////////////////////////////////////////////////////////////////

#include "nsMIMEInfoImpl.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsMIMEInfoImpl)

///////////////////////////////////////////////////////////////////////////////

#include "nsRequestObserverProxy.h"
#include "nsSimpleStreamListener.h"
#include "nsDirIndexParser.h"
#include "nsDirIndex.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(nsRequestObserverProxy)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSimpleStreamListener)
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsDirIndexParser, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDirIndex)

///////////////////////////////////////////////////////////////////////////////

#include "nsStreamListenerTee.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsStreamListenerTee)

///////////////////////////////////////////////////////////////////////////////

#include "nsHttpHandler.h"
#include "nsHttpAuthManager.h"
#include "nsHttpBasicAuth.h"
#include "nsHttpDigestAuth.h"
#ifdef XP_WIN
#include "nsHttpNTLMAuth.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsHttpNTLMAuth, Init)
#endif
#undef LOG

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsHttpHandler, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsHttpsHandler, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsHttpAuthManager, Init)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsHttpBasicAuth)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsHttpDigestAuth)

///////////////////////////////////////////////////////////////////////////////

#include "nsResProtocolHandler.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsResProtocolHandler, Init)

///////////////////////////////////////////////////////////////////////////////

#include "nsURIChecker.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsURIChecker)

///////////////////////////////////////////////////////////////////////////////

#include "nsURLParsers.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsNoAuthURLParser)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsAuthURLParser)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsStdURLParser)

#include "nsStandardURL.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsStandardURL)

///////////////////////////////////////////////////////////////////////////////

#include "nsResumableEntityID.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsResumableEntityID)

///////////////////////////////////////////////////////////////////////////////

#include "nsIDNService.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsIDNService)

///////////////////////////////////////////////////////////////////////////////

#include "nsFileProtocolHandler.h"
#include "nsJARProtocolHandler.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsFileProtocolHandler, Init)

#include "nsJARProtocolHandler.h"
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsJARProtocolHandler, Init)

#include "nsAboutProtocolHandler.h"
#include "nsAboutBlank.h"
#include "nsAboutBloat.h"
#include "nsAboutCache.h"
#include "nsAboutRedirector.h"

#include "nsAboutCacheEntry.h"
NS_GENERIC_FACTORY_CONSTRUCTOR(nsAboutCacheEntry)

///////////////////////////////////////////////////////////////////////////////

#include "nsFtpProtocolHandler.h"
#include "nsFTPDirListingConv.h"
#include "nsGopherDirListingConv.h"
#include "nsMultiMixedConv.h"
#include "nsHTTPCompressConv.h"
#include "mozTXTToHTMLConv.h"
#include "nsUnknownDecoder.h"
#include "nsTXTToHTMLConv.h"
#include "nsIndexedToHTML.h"
#ifdef BUILD_BINHEX_DECODER
#include "nsBinHexDecoder.h"
#endif

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsFtpProtocolHandler, Init);

nsresult NS_NewFTPDirListingConv(nsFTPDirListingConv** result);
nsresult NS_NewGopherDirListingConv(nsGopherDirListingConv** result);
nsresult NS_NewMultiMixedConv (nsMultiMixedConv** result);
nsresult MOZ_NewTXTToHTMLConv (mozTXTToHTMLConv** result);
nsresult NS_NewHTTPCompressConv  (nsHTTPCompressConv ** result);
nsresult NS_NewNSTXTToHTMLConv(nsTXTToHTMLConv** result);
nsresult NS_NewStreamConv(nsStreamConverterService **aStreamConv);

#define FTP_TO_INDEX                 "?from=text/ftp-dir&to=application/http-index-format"
#define GOPHER_TO_INDEX              "?from=text/gopher-dir&to=application/http-index-format"
#define INDEX_TO_HTML                "?from=application/http-index-format&to=text/html"
#define MULTI_MIXED_X                "?from=multipart/x-mixed-replace&to=*/*"
#define MULTI_MIXED                  "?from=multipart/mixed&to=*/*"
#define MULTI_BYTERANGES             "?from=multipart/byteranges&to=*/*"
#define UNKNOWN_CONTENT              "?from=application/x-unknown-content-type&to=*/*"
#define GZIP_TO_UNCOMPRESSED         "?from=gzip&to=uncompressed"
#define XGZIP_TO_UNCOMPRESSED        "?from=x-gzip&to=uncompressed"
#define COMPRESS_TO_UNCOMPRESSED     "?from=compress&to=uncompressed"
#define XCOMPRESS_TO_UNCOMPRESSED    "?from=x-compress&to=uncompressed"
#define DEFLATE_TO_UNCOMPRESSED      "?from=deflate&to=uncompressed"
#define PLAIN_TO_HTML                "?from=text/plain&to=text/html"

#ifdef BUILD_BINHEX_DECODER
#define BINHEX_TO_WILD               "?from=application/mac-binhex40&to=*/*"
#endif

static const char *const g_StreamConverterArray[] = {
        FTP_TO_INDEX,
        GOPHER_TO_INDEX,
        INDEX_TO_HTML,
        MULTI_MIXED_X,
        MULTI_MIXED,
        MULTI_BYTERANGES,
        UNKNOWN_CONTENT,
        GZIP_TO_UNCOMPRESSED,
        XGZIP_TO_UNCOMPRESSED,
        COMPRESS_TO_UNCOMPRESSED,
        XCOMPRESS_TO_UNCOMPRESSED,
        DEFLATE_TO_UNCOMPRESSED,
#ifdef BUILD_BINHEX_DECODER
        BINHEX_TO_WILD,
#endif
        PLAIN_TO_HTML
    };

static PRUint32 g_StreamConverterCount = sizeof(g_StreamConverterCount)/sizeof(const char*);

// each stream converter must add its from/to key to the category manager
// in RegisterStreamConverters(). This provides a string representation
// of each registered converter, rooted in the NS_ISTREAMCONVERTER_KEY
// category. These keys are then (when the stream converter service
// needs to chain converters together) enumerated and parsed to build
// a graph of converters for potential chaining.
static NS_METHOD
RegisterStreamConverters(nsIComponentManager *aCompMgr, nsIFile *aPath,
                         const char *registryLocation,
                         const char *componentType,
                         const nsModuleComponentInfo *info) {
    nsresult rv;
    nsCOMPtr<nsICategoryManager> catmgr =
        do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv);
    if (NS_FAILED(rv)) return rv;
    nsXPIDLCString previous;

    PRUint32 count = 0;
    while (count < g_StreamConverterCount) {
        (void) catmgr->AddCategoryEntry(NS_ISTREAMCONVERTER_KEY, g_StreamConverterArray[count],
                                      "", PR_TRUE, PR_TRUE, getter_Copies(previous));
        if (NS_FAILED(rv)) NS_ASSERTION(0, "adding a cat entry failed");
        count++;
    }
    
    return rv;
}

// same as RegisterStreamConverters except the reverse.
static NS_METHOD
UnregisterStreamConverters(nsIComponentManager *aCompMgr, nsIFile *aPath,
                           const char *registryLocation,
                           const nsModuleComponentInfo *info) {
    nsresult rv;
    nsCOMPtr<nsICategoryManager> catmgr =
        do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv);
    if (NS_FAILED(rv)) return rv;


    PRUint32 count = 0;
    while (count < g_StreamConverterCount) {
        rv = catmgr->DeleteCategoryEntry(NS_ISTREAMCONVERTER_KEY, 
                                         g_StreamConverterArray[count], 
                                         PR_TRUE);
        if (NS_FAILED(rv)) return rv;
        count++;
    }
    return rv;
}

#ifdef BUILD_BINHEX_DECODER
NS_GENERIC_FACTORY_CONSTRUCTOR(nsBinHexDecoder);
#endif

static NS_IMETHODIMP                 
CreateNewStreamConvServiceFactory(nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {                                                  
        return NS_ERROR_INVALID_POINTER;                             
    }
    if (aOuter) {                                                    
        *aResult = nsnull;                                           
        return NS_ERROR_NO_AGGREGATION;                              
    }   
    nsStreamConverterService* inst = nsnull;
    nsresult rv = NS_NewStreamConv(&inst);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
        return rv;                                                   
    } 
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
    }                                                                
    NS_RELEASE(inst);             /* get rid of extra refcnt */      
    return rv;              
}

static NS_IMETHODIMP                 
CreateNewFTPDirListingConv(nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {                                                  
        return NS_ERROR_INVALID_POINTER;                             
    }
    if (aOuter) {                                                    
        *aResult = nsnull;                                           
        return NS_ERROR_NO_AGGREGATION;                              
    }   
    nsFTPDirListingConv* inst = nsnull;
    nsresult rv = NS_NewFTPDirListingConv(&inst);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
        return rv;                                                   
    } 
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
    }                                                                
    NS_RELEASE(inst);             /* get rid of extra refcnt */      
    return rv;              
}

static NS_IMETHODIMP                 
CreateNewGopherDirListingConv(nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {
        return NS_ERROR_INVALID_POINTER;
    }
    if (aOuter) {
        *aResult = nsnull;
        return NS_ERROR_NO_AGGREGATION;
    }
    nsGopherDirListingConv* inst = nsnull;
    nsresult rv = NS_NewGopherDirListingConv(&inst);
    if (NS_FAILED(rv)) {
        *aResult = nsnull;
        return rv;
    }
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {
        *aResult = nsnull;
    }
    NS_RELEASE(inst);             /* get rid of extra refcnt */
    return rv;
}

static NS_IMETHODIMP                 
CreateNewMultiMixedConvFactory(nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {                                                  
        return NS_ERROR_INVALID_POINTER;                             
    }
    if (aOuter) {                                                    
        *aResult = nsnull;                                           
        return NS_ERROR_NO_AGGREGATION;                              
    }   
    nsMultiMixedConv* inst = nsnull;
    nsresult rv = NS_NewMultiMixedConv(&inst);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
        return rv;                                                   
    } 
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
    }                                                                
    NS_RELEASE(inst);             /* get rid of extra refcnt */      
    return rv;              
}

static NS_IMETHODIMP                 
CreateNewTXTToHTMLConvFactory(nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {                                                  
        return NS_ERROR_INVALID_POINTER;                             
    }
    if (aOuter) {                                                    
        *aResult = nsnull;                                           
        return NS_ERROR_NO_AGGREGATION;                              
    }   
    mozTXTToHTMLConv* inst = nsnull;
    nsresult rv = MOZ_NewTXTToHTMLConv(&inst);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
        return rv;                                                   
    } 
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
    }                                                                
    NS_RELEASE(inst);             /* get rid of extra refcnt */      
    return rv;              
}

static NS_IMETHODIMP                 
CreateNewHTTPCompressConvFactory (nsISupports* aOuter, REFNSIID aIID, void **aResult) 
{
    if (!aResult) {                                                  
        return NS_ERROR_INVALID_POINTER;                             
    }
    if (aOuter) {                                                    
        *aResult = nsnull;                                           
        return NS_ERROR_NO_AGGREGATION;                              
    }   
    nsHTTPCompressConv* inst = nsnull;
    nsresult rv = NS_NewHTTPCompressConv (&inst);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
        return rv;                                                   
    } 
    rv = inst->QueryInterface(aIID, aResult);
    if (NS_FAILED(rv)) {                                             
        *aResult = nsnull;                                           
    }                                                                
    NS_RELEASE(inst);             /* get rid of extra refcnt */      
    return rv;              
}

static NS_IMETHODIMP
CreateNewUnknownDecoderFactory(nsISupports *aOuter, REFNSIID aIID, void **aResult)
{
  nsresult rv;

  if (!aResult) {
    return NS_ERROR_NULL_POINTER;
  }
  *aResult = nsnull;

  if (aOuter) {
    return NS_ERROR_NO_AGGREGATION;
  }

  nsUnknownDecoder *inst;
  
  inst = new nsUnknownDecoder();
  if (!inst) {
    return NS_ERROR_OUT_OF_MEMORY;
  }
  NS_ADDREF(inst);
  rv = inst->QueryInterface(aIID, aResult);
  NS_RELEASE(inst);

  return rv;
}

static NS_IMETHODIMP
CreateNewNSTXTToHTMLConvFactory(nsISupports *aOuter, REFNSIID aIID, void **aResult)
{
  nsresult rv;

  if (!aResult) {
    return NS_ERROR_NULL_POINTER;
  }
  *aResult = nsnull;

  if (aOuter) {
    return NS_ERROR_NO_AGGREGATION;
  }

  nsTXTToHTMLConv *inst;
  
  inst = new nsTXTToHTMLConv();
  if (!inst) return NS_ERROR_OUT_OF_MEMORY;

  NS_ADDREF(inst);
  rv = inst->Init();
  if (NS_FAILED(rv)) {
    delete inst;
    return rv;
  }
  rv = inst->QueryInterface(aIID, aResult);
  NS_RELEASE(inst);

  return rv;
}

///////////////////////////////////////////////////////////////////////////////
// Module implementation for the net library

// Necko module shutdown hook
static void PR_CALLBACK nsNeckoShutdown(nsIModule *neckoModule)
{
    // Release the url parser that the stdurl is holding.
    nsStandardURL::ShutdownGlobalObjects();

    // Release buffer cache
    NS_IF_RELEASE(nsIOService::gBufferCache);

    // Release global state used by the URL helper module.
    net_ShutdownURLHelper();
}

static const nsModuleComponentInfo gNetModuleInfo[] = {
    { NS_IOSERVICE_CLASSNAME,
      NS_IOSERVICE_CID,
      NS_IOSERVICE_CONTRACTID,
      nsIOServiceConstructor },
    { NS_STREAMTRANSPORTSERVICE_CLASSNAME,
      NS_STREAMTRANSPORTSERVICE_CID,
      NS_STREAMTRANSPORTSERVICE_CONTRACTID,
      nsStreamTransportServiceConstructor },
    { NS_SOCKETTRANSPORTSERVICE_CLASSNAME,
      NS_SOCKETTRANSPORTSERVICE_CID,
      NS_SOCKETTRANSPORTSERVICE_CONTRACTID,
      nsSocketTransportServiceConstructor },
    { "Socket Provider Service", 
      NS_SOCKETPROVIDERSERVICE_CID,
      "@mozilla.org/network/socket-provider-service;1",
      nsSocketProviderService::Create },
    { "DNS Service", 
      NS_DNSSERVICE_CID,
      "@mozilla.org/network/dns-service;1",
      nsDNSService::Create },
    { NS_IDNSERVICE_CLASSNAME,
      NS_IDNSERVICE_CID,
      NS_IDNSERVICE_CONTRACTID,
      nsIDNServiceConstructor },
    { NS_SIMPLEURI_CLASSNAME,
      NS_SIMPLEURI_CID,
      NS_SIMPLEURI_CONTRACTID,
      nsSimpleURI::Create },
    { "External Module Manager", 
      NS_NETMODULEMGR_CID,
      "@mozilla.org/network/net-extern-mod;1",
      nsNetModuleMgr::Create },
    { NS_ASYNCSTREAMCOPIER_CLASSNAME,
      NS_ASYNCSTREAMCOPIER_CID,
      NS_ASYNCSTREAMCOPIER_CONTRACTID,
      nsAsyncStreamCopierConstructor },
    { NS_INPUTSTREAMPUMP_CLASSNAME,
      NS_INPUTSTREAMPUMP_CID,
      NS_INPUTSTREAMPUMP_CONTRACTID,
      nsInputStreamPumpConstructor },
    { NS_INPUTSTREAMCHANNEL_CLASSNAME,
      NS_INPUTSTREAMCHANNEL_CID,
      NS_INPUTSTREAMCHANNEL_CONTRACTID,
      nsInputStreamChannelConstructor },
    { NS_STREAMLOADER_CLASSNAME, 
      NS_STREAMLOADER_CID,
      NS_STREAMLOADER_CONTRACTID,
      nsStreamLoader::Create },
    { NS_UNICHARSTREAMLOADER_CLASSNAME, 
      NS_UNICHARSTREAMLOADER_CID,
      NS_UNICHARSTREAMLOADER_CONTRACTID,
      nsUnicharStreamLoader::Create },
    { NS_DOWNLOADER_CLASSNAME,
      NS_DOWNLOADER_CID,
      NS_DOWNLOADER_CONTRACTID,
      nsDownloader::Create },
    { NS_REQUESTOBSERVERPROXY_CLASSNAME,
      NS_REQUESTOBSERVERPROXY_CID,
      NS_REQUESTOBSERVERPROXY_CONTRACTID,
      nsRequestObserverProxyConstructor },
    { NS_SIMPLESTREAMLISTENER_CLASSNAME,
      NS_SIMPLESTREAMLISTENER_CID,
      NS_SIMPLESTREAMLISTENER_CONTRACTID,
      nsSimpleStreamListenerConstructor },
    { NS_ASYNCSTREAMLISTENER_CLASSNAME,
      NS_ASYNCSTREAMLISTENER_CID,
      NS_ASYNCSTREAMLISTENER_CONTRACTID,
      nsAsyncStreamListener::Create },
    { NS_STREAMLISTENERTEE_CLASSNAME,
      NS_STREAMLISTENERTEE_CID,
      NS_STREAMLISTENERTEE_CONTRACTID,
      nsStreamListenerTeeConstructor },
    { NS_LOADGROUP_CLASSNAME,
      NS_LOADGROUP_CID,
      NS_LOADGROUP_CONTRACTID,
      nsLoadGroup::Create },
    { NS_LOCALFILEINPUTSTREAM_CLASSNAME, 
      NS_LOCALFILEINPUTSTREAM_CID,
      NS_LOCALFILEINPUTSTREAM_CONTRACTID,
      nsFileInputStream::Create },
    { NS_LOCALFILEOUTPUTSTREAM_CLASSNAME, 
      NS_LOCALFILEOUTPUTSTREAM_CID,
      NS_LOCALFILEOUTPUTSTREAM_CONTRACTID,
      nsFileOutputStream::Create },
    
    { "URIChecker",
      NS_URICHECKER_CID,
      NS_URICHECKER_CONTRACT_ID,
      nsURICheckerConstructor
    },

    { NS_RESUMABLEENTITYID_CLASSNAME,
      NS_RESUMABLEENTITYID_CID,
      NS_RESUMABLEENTITYID_CONTRACTID,
      nsResumableEntityIDConstructor
    },

    // The register functions for the built-in 
    // parsers just need to be called once.
    { NS_STDURLPARSER_CLASSNAME,
      NS_STDURLPARSER_CID,
      NS_STDURLPARSER_CONTRACTID,
      nsStdURLParserConstructor},
    { NS_NOAUTHURLPARSER_CLASSNAME,
      NS_NOAUTHURLPARSER_CID,
      NS_NOAUTHURLPARSER_CONTRACTID,
      nsNoAuthURLParserConstructor },
    { NS_AUTHURLPARSER_CLASSNAME,
      NS_AUTHURLPARSER_CID,
      NS_AUTHURLPARSER_CONTRACTID,
      nsAuthURLParserConstructor },

    { NS_STANDARDURL_CLASSNAME,
      NS_STANDARDURL_CID,
      NS_STANDARDURL_CONTRACTID,
      nsStandardURLConstructor },

    { NS_BUFFEREDINPUTSTREAM_CLASSNAME, 
      NS_BUFFEREDINPUTSTREAM_CID,
      NS_BUFFEREDINPUTSTREAM_CONTRACTID,
      nsBufferedInputStream::Create },
    { NS_BUFFEREDOUTPUTSTREAM_CLASSNAME, 
      NS_BUFFEREDOUTPUTSTREAM_CID,
      NS_BUFFEREDOUTPUTSTREAM_CONTRACTID,
      nsBufferedOutputStream::Create },
    { NS_MIMEINPUTSTREAM_CLASSNAME,
      NS_MIMEINPUTSTREAM_CID,
      NS_MIMEINPUTSTREAM_CONTRACTID,
      nsMIMEInputStreamConstructor },
    { "Protocol Proxy Service",
      NS_PROTOCOLPROXYSERVICE_CID,
      "@mozilla.org/network/protocol-proxy-service;1",
      nsProtocolProxyService::Create },

    // from netwerk/streamconv:

    // this converter is "always" built.
    // HACK-ALERT, register *all* converters
    // in this converter's component manager 
    // registration. just piggy backing here until
    // you can add registration functions to
    // the generic module macro.
    { "Stream Converter Service", 
      NS_STREAMCONVERTERSERVICE_CID,
      "@mozilla.org/streamConverters;1", 
      CreateNewStreamConvServiceFactory,
      RegisterStreamConverters,   // registers *all* converters
      UnregisterStreamConverters  // unregisters *all* converters
    },
    
#ifdef BUILD_APPLEFILE_DECODER
    { NS_APPLEFILEDECODER_CLASSNAME, 
      NS_APPLEFILEDECODER_CID,
      NS_IAPPLEFILEDECODER_CONTRACTID, 
      nsAppleFileDecoderConstructor
    },
#endif

    // from netwerk/streamconv/converters:
    { "FTPDirListingConverter", 
      NS_FTPDIRLISTINGCONVERTER_CID, 
      NS_ISTREAMCONVERTER_KEY FTP_TO_INDEX, 
      CreateNewFTPDirListingConv
    },

    { "GopherDirListingConverter",
      NS_GOPHERDIRLISTINGCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY GOPHER_TO_INDEX,
      CreateNewGopherDirListingConv
    },    

    { "Indexed to HTML Converter", 
      NS_NSINDEXEDTOHTMLCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY INDEX_TO_HTML, 
      nsIndexedToHTML::Create
    },

    { "Directory Index Parser",
      NS_DIRINDEXPARSER_CID,
      NS_DIRINDEXPARSER_CONTRACTID,
      nsDirIndexParserConstructor
    },

    { "MultiMixedConverter", 
      NS_MULTIMIXEDCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY MULTI_MIXED_X, 
      CreateNewMultiMixedConvFactory
    },

    { "MultiMixedByteRange", 
      NS_MULTIMIXEDCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY MULTI_BYTERANGES, 
      CreateNewMultiMixedConvFactory
    },

    { "MultiMixedConverter2",
      NS_MULTIMIXEDCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY MULTI_MIXED,
      CreateNewMultiMixedConvFactory
    },

    { "Unknown Content-Type Decoder",
      NS_UNKNOWNDECODER_CID,
      NS_ISTREAMCONVERTER_KEY UNKNOWN_CONTENT,
      CreateNewUnknownDecoderFactory
    },

    { "HttpCompressConverter", 
      NS_HTTPCOMPRESSCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY GZIP_TO_UNCOMPRESSED,
      CreateNewHTTPCompressConvFactory
    },

    { "HttpCompressConverter", 
      NS_HTTPCOMPRESSCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY XGZIP_TO_UNCOMPRESSED,
      CreateNewHTTPCompressConvFactory
    },
    { "HttpCompressConverter", 
      NS_HTTPCOMPRESSCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY COMPRESS_TO_UNCOMPRESSED,
      CreateNewHTTPCompressConvFactory
    },
    { "HttpCompressConverter", 
      NS_HTTPCOMPRESSCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY XCOMPRESS_TO_UNCOMPRESSED,
      CreateNewHTTPCompressConvFactory
    },
    { "HttpCompressConverter", 
      NS_HTTPCOMPRESSCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY DEFLATE_TO_UNCOMPRESSED,
      CreateNewHTTPCompressConvFactory
    },
    { "NSTXTToHTMLConverter",
      NS_NSTXTTOHTMLCONVERTER_CID,
      NS_ISTREAMCONVERTER_KEY PLAIN_TO_HTML,
      CreateNewNSTXTToHTMLConvFactory
    },
#ifdef BUILD_BINHEX_DECODER
    { "nsBinHexConverter", NS_BINHEXDECODER_CID,
      NS_ISTREAMCONVERTER_KEY BINHEX_TO_WILD,
      nsBinHexDecoderConstructor
    },
#endif
	// This is not a real stream converter, it's just
	// registering it's cid factory here.
	{ "HACK-TXTToHTMLConverter", 
  	  MOZITXTTOHTMLCONV_CID,
	  MOZ_TXTTOHTMLCONV_CONTRACTID, 
	  CreateNewTXTToHTMLConvFactory
    },

    { "Directory Index",
      NS_DIRINDEX_CID,
      "@mozilla.org/dirIndex;1",
      nsDirIndexConstructor
    },

    // from netwerk/mime:
    { "xml mime INFO", 
      NS_MIMEINFO_CID,
      NS_MIMEINFO_CONTRACTID,
      nsMIMEInfoImplConstructor
    },

    // from netwerk/protocol/file:
    { NS_FILEPROTOCOLHANDLER_CLASSNAME,
      NS_FILEPROTOCOLHANDLER_CID,  
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "file", 
      nsFileProtocolHandlerConstructor
    },

    { "HTTP Handler",
      NS_HTTPPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "http",
      nsHttpHandlerConstructor },

    { "HTTPS Handler",
      NS_HTTPSPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "https",
      nsHttpsHandlerConstructor },

    { "HTTP Basic Auth Encoder",
      NS_HTTPBASICAUTH_CID,
      NS_HTTP_AUTHENTICATOR_CONTRACTID_PREFIX "basic",
      nsHttpBasicAuthConstructor },

    { "HTTP Digest Auth Encoder",
      NS_HTTPDIGESTAUTH_CID,
      NS_HTTP_AUTHENTICATOR_CONTRACTID_PREFIX "digest",
      nsHttpDigestAuthConstructor },

#ifdef XP_WIN
    { "HTTP NTLM Auth Encoder",
      NS_HTTPNTLMAUTH_CID,
      NS_HTTP_AUTHENTICATOR_CONTRACTID_PREFIX "ntlm",
      nsHttpNTLMAuthConstructor },
#endif

    { NS_HTTPAUTHMANAGER_CLASSNAME,
      NS_HTTPAUTHMANAGER_CID,
      NS_HTTPAUTHMANAGER_CONTRACTID,
      nsHttpAuthManagerConstructor },
      
    // from netwerk/protocol/ftp:
    { "The FTP Protocol Handler", 
      NS_FTPPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "ftp",
      nsFtpProtocolHandlerConstructor
    },

    // from netwerk/protocol/jar:
    { NS_JARPROTOCOLHANDLER_CLASSNAME,
      NS_JARPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "jar", 
      nsJARProtocolHandlerConstructor
    },

    // from netwerk/protocol/res:
    { NS_RESPROTOCOLHANDLER_CLASSNAME,
      NS_RESPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "resource",
      nsResProtocolHandlerConstructor
    },

    // from netwerk/protocol/about:
    { "About Protocol Handler", 
      NS_ABOUTPROTOCOLHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "about", 
      nsAboutProtocolHandler::Create
    },
    { "about:blank", 
      NS_ABOUT_BLANK_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "blank", 
      nsAboutBlank::Create
    },
    { "about:bloat", 
      NS_ABOUT_BLOAT_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "bloat", 
      nsAboutBloat::Create
    },
    { "about:config",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "config",
      nsAboutRedirector::Create
    },
    { "about:credits",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "credits",
      nsAboutRedirector::Create
    },
    { "about:plugins",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "plugins",
      nsAboutRedirector::Create
    },
    { "about:mozilla",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "mozilla",
      nsAboutRedirector::Create
    },
    { "about:logo",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "logo",
      nsAboutRedirector::Create
    },
    { "about:buildconfig",
      NS_ABOUT_REDIRECTOR_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "buildconfig",
      nsAboutRedirector::Create
    },

    { "about:cache", 
      NS_ABOUT_CACHE_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "cache", 
      nsAboutCache::Create
    },
    { "about:cache-entry",
      NS_ABOUT_CACHE_ENTRY_MODULE_CID,
      NS_ABOUT_MODULE_CONTRACTID_PREFIX "cache-entry",
      nsAboutCacheEntryConstructor
    },

    {  NS_ISOCKSSOCKETPROVIDER_CLASSNAME,
       NS_SOCKSSOCKETPROVIDER_CID,
       NS_ISOCKSSOCKETPROVIDER_CONTRACTID,
       nsSOCKSSocketProvider::Create
    },

    {  NS_ISOCKS4SOCKETPROVIDER_CLASSNAME,
       NS_SOCKS4SOCKETPROVIDER_CID,
       NS_ISOCKS4SOCKETPROVIDER_CONTRACTID,
       nsSOCKS4SocketProvider::Create
    },

    {  NS_CACHESERVICE_CLASSNAME,
       NS_CACHESERVICE_CID,
       NS_CACHESERVICE_CONTRACTID,
       nsCacheService::Create
    },

};

NS_IMPL_NSGETMODULE_WITH_DTOR(necko_core_and_primary_protocols, gNetModuleInfo,
                              nsNeckoShutdown)
