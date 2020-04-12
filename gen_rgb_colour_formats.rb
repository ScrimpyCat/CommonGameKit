tab = ' '*4

types = {
    'Unorm_sRGB' => 'CCColourFormatSpaceRGB_sRGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized',
    'Unorm' => 'CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized',
    'Snorm' => 'CCColourFormatSpaceRGB_RGB | CCColourFormatTypeSignedInteger | CCColourFormatNormalized',
    'Uint' => 'CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger',
    'Sint' => 'CCColourFormatSpaceRGB_RGB | CCColourFormatTypeSignedInteger',
    'Float' => 'CCColourFormatSpaceRGB_RGB | CCColourFormatTypeFloat'
}

names = [
    'R8Unorm',
    'R8Unorm_sRGB',
    'R8Snorm',
    'R8Uint',
    'R8Sint',
    'G8Unorm',
    'G8Unorm_sRGB',
    'G8Snorm',
    'G8Uint',
    'G8Sint',
    'B8Unorm',
    'B8Unorm_sRGB',
    'B8Snorm',
    'B8Uint',
    'B8Sint',
    'A8Unorm',
    'A8Snorm',
    'A8Uint',
    'A8Sint',
    'R16Unorm',
    'R16Snorm',
    'R16Uint',
    'R16Sint',
    'R16Float',
    'G16Unorm',
    'G16Snorm',
    'G16Uint',
    'G16Sint',
    'G16Float',
    'B16Unorm',
    'B16Snorm',
    'B16Uint',
    'B16Sint',
    'B16Float',
    'A16Unorm',
    'A16Snorm',
    'A16Uint',
    'A16Sint',
    'A16Float',
    'RG8Unorm',
    'RG8Unorm_sRGB',
    'RG8Snorm',
    'RG8Uint',
    'RG8Sint',
    'R5G6B5Unorm',
    'R5G6B5Snorm',
    'R5G6B5Uint',
    'R5G6B5Sint',
    'B5G6R5Unorm',
    'B5G6R5Snorm',
    'B5G6R5Uint',
    'B5G6R5Sint',
    'RGB5A1Unorm',
    'RGB5A1Snorm',
    'RGB5A1Uint',
    'RGB5A1Sint',
    'BGR5A1Unorm',
    'BGR5A1Snorm',
    'BGR5A1Uint',
    'BGR5A1Sint',
    'A1RGB5Unorm',
    'A1RGB5Snorm',
    'A1RGB5Uint',
    'A1RGB5Sint',
    'A1BGR5Unorm',
    'A1BGR5Snorm',
    'A1BGR5Uint',
    'A1BGR5Sint',
    'RGBA4Unorm',
    'RGBA4Snorm',
    'RGBA4Uint',
    'RGBA4Sint',
    'BGRA4Unorm',
    'BGRA4Snorm',
    'BGRA4Uint',
    'BGRA4Sint',
    'ARGB4Unorm',
    'ARGB4Snorm',
    'ARGB4Uint',
    'ARGB4Sint',
    'ABGR4Unorm',
    'ABGR4Snorm',
    'ABGR4Uint',
    'ABGR4Sint',
    'RGB8Unorm',
    'RGB8Unorm_sRGB',
    'RGB8Snorm',
    'RGB8Uint',
    'RGB8Sint',
    'BGR8Unorm',
    'BGR8Unorm_sRGB',
    'BGR8Snorm',
    'BGR8Uint',
    'BGR8Sint',
    'R32Uint',
    'R32Sint',
    'R32Float',
    'G32Uint',
    'G32Sint',
    'G32Float',
    'B32Uint',
    'B32Sint',
    'B32Float',
    'A32Uint',
    'A32Sint',
    'A32Float',
    'RG16Unorm',
    'RG16Snorm',
    'RG16Uint',
    'RG16Sint',
    'RG16Float',
    'RGBA8Unorm',
    'RGBA8Unorm_sRGB',
    'RGBA8Snorm',
    'RGBA8Uint',
    'RGBA8Sint',
    'BGRA8Unorm',
    'BGRA8Unorm_sRGB',
    'BGRA8Snorm',
    'BGRA8Uint',
    'BGRA8Sint',
    'ARGB8Unorm',
    'ARGB8Unorm_sRGB',
    'ARGB8Snorm',
    'ARGB8Uint',
    'ARGB8Sint',
    'ABGR8Unorm',
    'ABGR8Unorm_sRGB',
    'ABGR8Snorm',
    'ABGR8Uint',
    'ABGR8Sint',
    'RG32Uint',
    'RG32Sint',
    'RG32Float',
    'RGBA16Unorm',
    'RGBA16Snorm',
    'RGBA16Uint',
    'RGBA16Sint',
    'RGBA16Float',
    'BGRA16Unorm',
    'BGRA16Snorm',
    'BGRA16Uint',
    'BGRA16Sint',
    'BGRA16Float',
    'ARGB16Unorm',
    'ARGB16Snorm',
    'ARGB16Uint',
    'ARGB16Sint',
    'ARGB16Float',
    'ABGR16Unorm',
    'ABGR16Snorm',
    'ABGR16Uint',
    'ABGR16Sint',
    'ABGR16Float',
    'RGB32Uint',
    'RGB32Sint',
    'RGB32Float',
    'BGR32Uint',
    'BGR32Sint',
    'BGR32Float',
    'RGBA32Uint',
    'RGBA32Sint',
    'RGBA32Float',
    'BGRA32Uint',
    'BGRA32Sint',
    'BGRA32Float',
    'ARGB32Uint',
    'ARGB32Sint',
    'ARGB32Float',
    'ABGR32Uint',
    'ABGR32Sint',
    'ABGR32Float'
]


def colour_format_channel_type(channel, bits, offset, space)
    "CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannel#{channel}, #{' '*(space-channel.length)}#{bits}, CCColourFormatChannelOffset#{offset})"
end

enums = ''
bc = 0
chan = ''
names.each { |name|
    type = name[/(?<=\d)+\D+\z/]

    definition = tab + 'CCColourFormat' + name

    c = definition.length % tab.length
    odd = ((definition.length + (tab.length - c)) / tab.length) % 2
    t = (tab * (1 + odd))
    t = t[0...(t.length - c)]

    cn = name.chomp(type)
    if cn[/8/] and (tab + cn).length % tab.length == 1
        if type == 'Unorm_sRGB'
            t = t.chomp(tab)
        else
            t << tab
        end
    end

    definition << t + '= '

    c = (definition.length + 2) / tab.length

    definition << types[type] + "\n"

    cs = 0
    if cn =~ /(A|G)/
        cs = 5
    elsif cn =~ /B/
        cs = 4
    elsif cn =~ /R/
        cs = 3
    end

    b = 0
    index = 0
    name.scan(/\D+\d+/).each { |channel_size|
        bits = channel_size[/\d+/]
        channel_size.scan(/\D/).map { |channel|
            b += bits.to_i
            definition << tab*c + '| ' + colour_format_channel_type({ 'R' => 'Red', 'G' => 'Green', 'B' => 'Blue', 'A' => 'Alpha' }[channel], bits, index, cs) + "\n"
            index += 1
        }
    }

    if b != bc
        enums << "\n\n"*(bc > 0 ? 1 : 0)
        enums << tab + "//#{b}-bit\n"
        bc = b
        chan = ''
    end

    if chan != cn
        enums << "\n\n"*(chan.length > 0 ? 1 : 0)
        enums << tab + "//#{cn}\n"
        chan = cn
    end

    enums << definition.chomp + ",\n"
}


src = """
//This file is automatically generated, modifications will be lost!
typedef CC_ENUM(CCColourFormatRGB, uint64_t) {
#{enums.chomp}
};
""".lstrip

File.write('src/rendering/utilities/colour/ColourFormatRGB.h', src)
