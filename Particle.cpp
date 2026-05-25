{
	"artifacts" : 
	[
		{
			"path" : "BuckshotRoulette.exe"
		},
		{
			"path" : "BuckshotRoulette.pdb"
		}
	],
	"backtrace" : 1,
	"backtraceGraph" : 
	{
		"commands" : 
		[
			"add_executable",
			"install",
			"target_link_libraries",
			"add_compile_options",
			"target_include_directories"
		],
		"files" : 
		[
			"CMakeLists.txt"
		],
		"nodes" : 
		[
			{
				"file" : 0
			},
			{
				"command" : 0,
				"file" : 0,
				"line" : 35,
				"parent" : 0
			},
			{
				"command" : 1,
				"file" : 0,
				"line" : 55,
				"parent" : 0
			},
			{
				"command" : 2,
				"file" : 0,
				"line" : 39,
				"parent" : 0
			},
			{
				"command" : 3,
				"file" : 0,
				"line" : 8,
				"parent" : 0
			},
			{
				"command" : 4,
				"file" : 0,
				"line" : 37,
				"parent" : 0
			}
		]
	},
	"codemodelVersion" : 
	{
		"major" : 2,
		"minor" : 9
	},
	"compileDependencies" : 
	[
		{
			"backtrace" : 3,
			"id" : "sfml-graphics::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-window::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-audio::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-system::@6890427a1f51a3e7e1df"
		}
	],
	"compileGroups" : 
	[
		{
			"compileCommandFragments" : 
			[
				{
					"fragment" : "/DWIN32 /D_WINDOWS /GR /EHsc /Zi /Ob0 /Od /RTC1 -std:c++17 -MDd"
				},
				{
					"backtrace" : 4,
					"fragment" : "/utf-8"
				}
			],
			"includes" : 
			[
				{
					"backtrace" : 5,
					"path" : "C:/Users/camka/OneDrive/Desktop/buckshot_roulette/include"
				},
				{
					"backtrace" : 3,
					"isSystem" : true,
					"path" : "C:/Users/camka/OneDrive/Desktop/SFML-2.6.2/include"
				}
			],
			"language" : "CXX",
			"languageStandard" : 
			{
				"backtraces" : 
				[
					1
				],
				"standard" : "17"
			},
			"sourceIndexes" : 
			[
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9
			]
		}
	],
	"id" : "BuckshotRoulette::@6890427a1f51a3e7e1df",
	"install" : 
	{
		"destinations" : 
		[
			{
				"backtrace" : 2,
				"path" : "bin"
			}
		],
		"prefix" : 
		{
			"path" : "C:/Users/camka/OneDrive/Desktop/buckshot_roulette/out/install/x64-Debug"
		}
	},
	"link" : 
	{
		"commandFragments" : 
		[
			{
				"fragment" : "/DWIN32 /D_WINDOWS /GR /EHsc /Zi /Ob0 /Od /RTC1 -MDd",
				"role" : "flags"
			},
			{
				"fragment" : "/machine:x64 /debug /INCREMENTAL",
				"role" : "flags"
			},
			{
				"fragment" : "/subsystem:console",
				"role" : "flags"
			},
			{
				"backtrace" : 3,
				"fragment" : "C:\\Users\\camka\\OneDrive\\Desktop\\SFML-2.6.2\\lib\\sfml-graphics-d.lib",
				"role" : "libraries"
			},
			{
				"backtrace" : 3,
				"fragment" : "C:\\Users\\camka\\OneDrive\\Desktop\\SFML-2.6.2\\lib\\sfml-window-d.lib",
				"role" : "libraries"
			},
			{
				"backtrace" : 3,
				"fragment" : "C:\\Users\\camka\\OneDrive\\Desktop\\SFML-2.6.2\\lib\\sfml-audio-d.lib",
				"role" : "libraries"
			},
			{
				"backtrace" : 3,
				"fragment" : "C:\\Users\\camka\\OneDrive\\Desktop\\SFML-2.6.2\\lib\\sfml-system-d.lib",
				"role" : "libraries"
			},
			{
				"fragment" : "kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib",
				"role" : "libraries"
			}
		],
		"language" : "CXX"
	},
	"linkLibraries" : 
	[
		{
			"backtrace" : 3,
			"id" : "sfml-graphics::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-window::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-audio::@6890427a1f51a3e7e1df"
		},
		{
			"backtrace" : 3,
			"id" : "sfml-system::@6890427a1f51a3e7e1df"
		}
	],
	"name" : "BuckshotRoulette",
	"nameOnDisk" : "BuckshotRoulette.exe",
	"paths" : 
	{
		"build" : ".",
		"source" : "."
	},
	"sourceGroups" : 
	[
		{
			"name" : "Source Files",
			"sourceIndexes" : 
			[
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9
			]
		}
	],
	"sources" : 
	[
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/main.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Game.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/DealerAI.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Item.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Particle.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Shell.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Shotgun.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/SoundManager.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/UI.cpp",
			"sourceGroupIndex" : 0
		},
		{
			"backtrace" : 1,
			"compileGroupIndex" : 0,
			"path" : "src/Utils.cpp",
			"sourceGroupIndex" : 0
		}
	],
	"type" : "EXECUTABLE"
}
