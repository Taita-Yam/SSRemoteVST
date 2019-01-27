

class HostParameter : public AudioProcessorParameter

{

public:
    
    HostParameter (float defaultParameterValue, const String& paramName)
    : defaultValue (defaultParameterValue),
    value (defaultParameterValue),
    name (paramName)

    {
    }
    
	~HostParameter() {}

    float getValue() const override
    {
        return value;
    }
    
    void setValue (float newValue) override
    {
        value = newValue;

    }
    
    float getDefaultValue() const override
    {
        return defaultValue;
    }
    
    String getName (int maximumStringLength) const override
    {
        return name;
    }
    
    
private:
    float defaultValue, value;
    String name;
};

