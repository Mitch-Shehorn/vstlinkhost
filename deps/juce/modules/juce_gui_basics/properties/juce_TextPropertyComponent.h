/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    A PropertyComponent that shows its value as editable text.

    @see PropertyComponent

    @tags{GUI}
*/
class JUCE_API  TextPropertyComponent  : public PropertyComponent
{
protected:
    //==============================================================================
    /** Creates a text property component.

        @param propertyName  The name of the property
        @param maxNumChars   If not zero, then this specifies the maximum allowable length of
                             the string. If zero, then the string will have no length limit.
        @param isMultiLine   Sets whether the text editor allows carriage returns.
        @param isEditable    Sets whether the text editor is editable. The default is true.

        @see TextEditor, setEditable
    */
    TextPropertyComponent (const String& propertyName,
                           int maxNumChars,
                           bool isMultiLine,
                           bool isEditable = true);

public:
    /** Creates a text property component.

        @param valueToControl The Value that is controlled by the TextPropertyComponent
        @param propertyName   The name of the property
        @param maxNumChars    If not zero, then this specifies the maximum allowable length of
                              the string. If zero, then the string will have no length limit.
        @param isMultiLine    Sets whether the text editor allows carriage returns.
        @param isEditable     Sets whether the text editor is editable. The default is true.

        @see TextEditor, setEditable
    */
    TextPropertyComponent (const Value& valueToControl,
                           const String& propertyName,
                           int maxNumChars,
                           bool isMultiLine,
                           bool isEditable = true);

    /** Creates a text property component with a default value.

        @param valueToControl The ValueTreePropertyWithDefault that is controlled by the TextPropertyComponent.
        @param propertyName   The name of the property
        @param maxNumChars    If not zero, then this specifies the maximum allowable length of
                              the string. If zero, then the string will have no length limit.
        @param isMultiLine    Sets whether the text editor allows carriage returns.
        @param isEditable     Sets whether the text editor is editable. The default is true.

        @see TextEditor, setEditable
    */
    TextPropertyComponent (const ValueTreePropertyWithDefault& valueToControl,
                           const String& propertyName,
                           int maxNumChars,
                           bool isMultiLine,
                           bool isEditable = true);

    ~TextPropertyComponent() override;

    //==============================================================================
    /** Called when the user edits the text.

        Your subclass must use this callback to change the value of whatever item
        this property component represents.
    */
    virtual void setText (const String& newText);

    /** Returns the text that should be shown in the text editor. */
    virtual String getText() const;

    /** Returns the text that should be shown in the text editor as a Value object. */
    Value& getValue() const;

    //==============================================================================
    /** Returns true if the text editor allows carriage returns. */
    bool isTextEditorMultiLine() const noexcept    { return isMultiLine; }

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the component.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId          = 0x100e401,    /**< The colour to fill the background of the text area. */
        textColourId                = 0x100e402,    /**< The colour to use for the editable text. */
        outlineColourId             = 0x100e403,    /**< The colour to use to draw an outline around the text area. */
    };

    void colourChanged() override;

    //==============================================================================
    /** Used to receive callbacks for text changes */
    class JUCE_API Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;

        /** Called when text has finished being entered (i.e. not per keypress) has changed. */
        virtual void textPropertyComponentChanged (TextPropertyComponent*) = 0;
    };

    /** Registers a listener to receive events when this button's state changes.
        If the listener is already registered, this will not register it again.
        @see removeListener
    */
    void addListener (Listener* newListener);

    /** Removes a previously-registered button listener
        @see addListener
    */
    void removeListener (Listener* listener);

    //==============================================================================
    /** Sets whether the text property component can have files dropped onto it by an external application.

        The default setting for this is true but you may want to disable this behaviour if you derive
        from this class and want your subclass to respond to the file drag.
    */
    void setInterestedInFileDrag (bool isInterested);

    /** Sets whether the text editor is editable. The default setting for this is true. */
    void setEditable (bool isEditable);

    //==============================================================================
    /** @internal */
    void refresh() override;
    /** @internal */
    virtual void textWasEdited();

private:
    //==============================================================================
    void callListeners();
    void createEditor (int maxNumChars, bool isEditable);

    //==============================================================================
    class LabelComp;

    const bool isMultiLine;

    ValueTreePropertyWithDefault value;
    std::unique_ptr<LabelComp> textEditor;
    ListenerList<Listener> listenerList;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextPropertyComponent)
};


} // namespace juce
