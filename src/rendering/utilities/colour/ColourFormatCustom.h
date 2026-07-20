typedef CC_ENUM(CCColourFormatCustom, uint64_t) {
    CCColourFormatDepth16Unorm     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                       | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex0, 16, CCColourFormatChannelOffset0),
    
    CCColourFormatDepth32Float     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeFloat
                                       | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex0, 32, CCColourFormatChannelOffset0),
    
    CCColourFormatStencil8Uint     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeUnsignedInteger
                                       | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex1, 8, CCColourFormatChannelOffset0),
    
    CCColourFormatDepth24Unorm_Stencil8Uint     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex0, 24, CCColourFormatChannelOffset0)
                                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex1,  8, CCColourFormatChannelOffset1),
    
    CCColourFormatDepth32Float_Stencil8Uint     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeFloat
                                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex0, 32, CCColourFormatChannelOffset0)
                                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex1,  8, CCColourFormatChannelOffset1),
    
    CCColourFormatDepth32Float_Stencil8Uint_Pad24     = CCColourFormatSpaceCustom_0 | CCColourFormatTypeFloat
                                                         | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex0, 32, CCColourFormatChannelOffset0)
                                                         | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex1,  8, CCColourFormatChannelOffset1)
                                                         | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelIndex2, 24, CCColourFormatChannelOffset2),
};
