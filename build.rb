require 'fileutils'
require 'rbconfig'
require 'optparse'

def os
    case RbConfig::CONFIG['host_os']
    when /darwin|mac os/
        'mac'
    when /linux/
        'linux'
    when /mswin|msys|mingw|cygwin|bccwin|wince|emc/
        'win'
    else
        puts 'Unsupported platform'
        exit
    end
end

def filter_platforms(file, ext = '')
    platforms = ['mac', 'linux', 'win']
    platforms.delete(os())
    file.end_with? *platforms.map { |platform| platform + ext }
end

library_mapper = {
    opengl: {
        mac: '-framework OpenGL',
        win: '-lopengl32',
        linux: '-lGL'
    },
    apple: {
        mac: '-framework CoreFoundation -framework Foundation'
    },
    cocoa: {
        mac: '-framework Cocoa'
    },
    metal: {
        mac: '-framework Metal'
    },
    win32: {
        win: '-mwindows'
    }
}

platforms = {
    any: os(),
    mac: 'mac',
    win: 'win',
    linux: 'linux'
}

options = {
    executable: false,
    framework: [],
    dylib: [],
    static: [],
    library: [],
    header: [],
    system: [],
    define: []
}
target = true
OptionParser.new do |opts|
    opts.on('-p', '--platform=PLATFORM', [:any, :mac, :win, :linux]) { |platform|
        target = platforms[platform] == os()
    }
    opts.on('-e', '--executable') { |e| options[:executable] = true }
    opts.on('-f', '--framework=FRAMEWORK', String) { |framework|
        options[:framework] << framework if target
        options[:dylib] << ["#{framework}/#{File.basename(framework)}.dylib", framework] if target
    }
    opts.on('-d', '--dylib=LIB,HEADERS', Array) { |lib, headers|
        options[:dylib] << [lib, headers] if target
    }
    opts.on('-s', '--static=LIB,HEADERS', Array) { |lib, headers|
        options[:static] << [lib, headers] if target
    }
    opts.on('-l', '--library=SOURCE,[HEADERS]', Array) { |source, headers|
        options[:library] << [source, headers || source] if target
    }
    opts.on('-h', '--header=HEADERS', String) { |headers|
        options[:header] << headers if target
    }
    opts.on('-L', '--lib=LIB', String) { |lib|
        options[:system] << library_mapper[lib.to_sym][os().to_sym] if target
    }
    opts.on('-D', '--define=DEFINE', String) { |define|
        options[:define] << define if target
    }
end.parse!

build_dir = ENV['BUILD_DIR'] || 'build'
parent_dir = build_dir.gsub(/[^\/]+/, '..')
project = File.basename(Dir.pwd)
target = build_dir + '/' + project
target_headers = FileUtils.mkdir_p(build_dir + '/private')
target_headers = FileUtils.mkdir_p(target + '/' + project) if not options[:executable]
target_assets = FileUtils.mkdir_p(target + '/assets')

FileUtils.cp_r('assets', target)

public_headers = Dir["src/**/*.h"].reject { |file| file.end_with? '_Private.h' }
FileUtils.cp(public_headers, target_headers)

private_headers = Dir["src/**/*_Private.h"]
FileUtils.cp(private_headers, build_dir + '/private')

options[:framework].each { |framework| FileUtils.cp_r(framework, target) }

ext = os() == 'mac' ? '{c,m}' : 'c'
sources = Dir["src/**/*.#{ext}"].reject { |file| filter_platforms(file, '.c') }
sources += options[:library].map { |source, headers| Dir["#{source}/*.#{ext}"] }.flatten

compile = sources.map { |file|
"""
build #{file.gsub(/[\/ ]/, '_')}.o: cc #{parent_dir}/#{file.gsub(' ', '$ ')}
    flags = $flags #{file.start_with?('src/') && file.end_with?('.m') && os() == 'mac' ? '-fobjc-arc' : ''}
    headers = $headers
    libs = $libs
""".strip
}

objects = sources.map { |file| file.gsub(/[\/ ]/, '_') + '.o' }

output = options[:executable] ? project : "#{project}.dylib"

deps_headers = [
    options[:dylib].map { |lib, headers| "-I#{parent_dir}/#{headers}".gsub(' ', '\$ ') },
    options[:static].map { |lib, headers| "-I#{parent_dir}/#{headers}".gsub(' ', '\$ ') },
    options[:library].map { |source, headers| "-I#{parent_dir}/#{headers}".gsub(' ', '\$ ') },
    options[:header].map { |headers| "-I#{parent_dir}/#{headers}".gsub(' ', '\$ ') }
].join(' ')

deps_libs = [
    options[:dylib].map { |lib, headers| "#{parent_dir}/#{lib}".gsub(' ', '\$ ') },
    options[:static].map { |lib, headers| "#{parent_dir}/#{lib}".gsub(' ', '\$ ') },
    options[:system]
].join(' ')

flags = %W[
    -Wall
    -Wno-logical-not-parentheses
    -Wno-missing-braces
    -Wno-multichar
    -Wno-unknown-pragmas
    #{options[:define].map { |define| '-D' + define }.join(' ')}
]

build = """
rule cc
    command = cc $in -c -o $out $flags $headers -std=gnu11 #{os() == 'mac' ? '-fmodules' : ''}

rule ld
    command = cc $in $objects -o $out $libs $args

flags = #{flags.join(' ')}

header = -I./#{project} -I./#{project}/#{project} -I./private

headers = $header #{deps_headers}
libs = #{deps_libs}

#{compile.join("\n")}

build #{project}/#{output}: ld #{objects.join(' ')}
    libs = $libs
""".lstrip

if options[:executable]
    if os() == 'mac'
        rpaths = options[:framework].map { |framework|
            options[:dylib].reject { |lib, headers| not lib.start_with? framework }.map { |lib, headers|
                '-Xlinker -rpath -Xlinker @executable_path/' + lib.gsub(/^#{framework}/, File.basename(framework)).gsub(/\/[^\/]*?\.dylib/, '')
            }
        }.join(' ')

        if not rpaths.empty?
            build += "    args = #{rpaths}\n"
        end
    end
else
    build += "    args = -shared #{os() == 'mac' ? "-install_name @rpath/#{output}" : ''}\n"
end

File.write(build_dir + '/build.ninja', build)
