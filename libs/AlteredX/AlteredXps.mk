
AlteredXps.dll: dlldata.obj AlteredX_p.obj AlteredX_i.obj
	link /dll /out:AlteredXps.dll /def:AlteredXps.def /entry:DllMain dlldata.obj AlteredX_p.obj AlteredX_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del AlteredXps.dll
	@del AlteredXps.lib
	@del AlteredXps.exp
	@del dlldata.obj
	@del AlteredX_p.obj
	@del AlteredX_i.obj
