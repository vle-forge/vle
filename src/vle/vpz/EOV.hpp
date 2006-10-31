/** 
 * @file EOV.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:58:41 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_EOV_HPP
#define VLE_VPZ_EOV_HPP

#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    class Outputs;

    /** 
     * @brief The base class of eye of vle to build a hierarchy of visualization
     * plugin.
     */
    class EOVchild : public Base
    {
    public:
        EOVchild() { }

        virtual EOVchild* clone() const = 0;

        virtual ~EOVchild() { }
    };



    /** 
     * @brief Define an empty widget to build graphical user interface of the
     * EOV program.
     */
    class EOVempty : public EOVchild
    {
    public:
        EOVempty()
        { }

        virtual EOVchild* clone() const
        { return new EOVempty(*this); }

        virtual ~EOVempty()
        { }

        /** 
         * @brief Assert that elt is a empty eov child.
         * 
         * @param elt a node to the empty eov child.
         */
        virtual void init(xmlpp::Element* elt);

        /** 
         * @brief Add this XML tag under the node elt.
         * @code
         * <CHILD TYPE="empty" />
         * @endcode
         * 
         * @param elt the parent node.
         */
        virtual void write(xmlpp::Element* elt) const;
    };



    /** 
     * @brief A container to store EOVchild to build the hierarchy of
     * visualization plugin. Child can be, EOVempty, EOVplugin, EOVhpaned or
     * EOVvapend.
     */
    class EOVcontainer : public EOVchild
    {
    public:
        /** 
         * @brief Build a container with one empty widget.
         */
        EOVcontainer();

        EOVcontainer(const EOVcontainer& cont);

        EOVcontainer& operator=(const EOVcontainer& cont);

        /** 
         * @brief Build a container with specified empty widget. If s equal 0,
         * only one empty widget is build.
         * 
         * @param s number of widget.
         */
        EOVcontainer(size_t s);
        
        virtual EOVchild* clone() const
        { return new EOVcontainer(*this); }

        virtual ~EOVcontainer();

        /** 
         * @brief Load all child of the current tag. The current tag must be
         * a container like HPaned, VPaned or EOV.
         * @code
         * <CHILD TYPE="hpaned" >
         *  <CHILD TYPE="vpaned" >
         *   <CHILD TYPE="empty" />
         *   <CHILD TYPE="plugin" OUTPUT="x1" PLUGIN="plot" />
         *  </CHILD>
         *  <CHILD TYPE="empty" />
         * </CHILD>
         * @endcode
         * 
         * @param elt the container's node.
         */
        void init(xmlpp::Element* elt);
    
        /** 
         * @brief Write under the node elt all container. The elt element is
         * not modified, only child.
         * 
         * @param elt the container's node.
         */
        void write(xmlpp::Element* elt) const;

        /** 
         * @brief Delete all existing EOVchild and resize vector.
         * 
         * @param sz The number of EOVchild.
         *
         * @throw Exception::Internal if sz == 0.
         */
        void setContainerSize(size_t sz);

        /** 
         * @brief Get the size of the current vector.
         * 
         * @return A number > 0.
         */
        size_t getContainerSize() const;

        /** 
         * @brief Set an empty child at number position of vector.
         * 
         * @param number the position of child into the vector.
         *
         * @throw Exception::Internal if number >= getContainerSize().
         */
        void setEmptyChild(size_t number);

        /** 
         * @brief Set a plugin child at number position of vector.
         * 
         * @param number the position of child into the vector.
         *
         * @throw Exception::Internal if number >= getContainerSize().
         */
        void setPluginChild(size_t number);

        /** 
         * @brief Set an hpaned child at number position of vector.
         * 
         * @param number the position of child into the vector.
         *
         * @throw Exception::Internal if number >= getContainerSize().
         */
        void setHPanedChild(size_t number);

        /** 
         * @brief Set a vpaned child at number position of vector.
         * 
         * @param number the position of child into the vector.
         *
         * @throw Exception::Internal if number >= getContainerSize().
         */
        void setVPanedChild(size_t number);

        /** 
         * @brief Get a reference to the child at number position in vector.
         * 
         * @param number the position of child into the vector.
         * 
         * @return the child.
         *
         * @throw Exception::Internal if number >= getContainerSize or if child
         * is null.
         */
        EOVchild& getChild(size_t number);

        /** 
         * @brief Get a reference to the child at number position in vector.
         * 
         * @param number the position of child into the vector.
         * 
         * @return the child.
         *
         * @throw Exception::Internal if number >= getContainerSize or if child
         * is null.
         */
        const EOVchild& getChild(size_t number) const;

    protected:
        std::vector < EOVchild* > m_lst;
    };



    /** 
     * @brief A container of two eov child horizontaly separated.
     */
    class EOVvpaned : public EOVcontainer
    {
    public:
        EOVvpaned() :
            EOVcontainer(2)
        { }

        virtual ~EOVvpaned()
        { }

        virtual EOVchild* clone() const
        { return new EOVvpaned(*this); }

        virtual void init(xmlpp::Element* elt);
        
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set an empty widget as left child.
         */
        void setEmptyLeftChild() { setEmptyChild(0); }
        
        /** 
         * @brief Set an hpaned widget as left child.
         */
        void setHPanedLeftChild() { setHPanedChild(0); }
        
        /** 
         * @brief Set a vpaned widget as left child.
         */
        void setVPanedLeftChild() { setVPanedChild(0); }
        
        /** 
         * @brief Set a plugin as left child.
         */
        void setPluginLeftChild() { setPluginChild(0); }
        
        /** 
         * @brief Set an empty widget as right child.
         */
        void setEmptyRightChild() { setEmptyChild(1); }
        
        /** 
         * @brief Set an hpaned widget as right child.
         */
        void setHPanedRightChild() { setHPanedChild(1); }
        
        /** 
         * @brief Set a vpaned widget as right child.
         */
        void setVPanedRightChild() { setVPanedChild(1); }

        /** 
         * @brief Set a plugin as right child.
         */
        void setPluginRightChild() { setPluginChild(1); }
        
        EOVchild& left() { return getChild(0); }
        
        const EOVchild& left() const { return getChild(0); }
        
        EOVchild& right() { return getChild(1); }
        
        const EOVchild& right() const { return getChild(1); }
    };



    /** 
     * @brief A container of two eov child horizontaly separated.
     */
    class EOVhpaned : public EOVcontainer
    {
    public:
        EOVhpaned() :
            EOVcontainer(2)
        { }

        virtual ~EOVhpaned()
        { }
        

        virtual EOVchild* clone() const
        { return new EOVhpaned(*this); }

        virtual void init(xmlpp::Element* elt);
        
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set an empty widget as top child.
         */
        void setEmptyTopChild() { setEmptyChild(0); }

        /** 
         * @brief Set a hpaned widget as top child.
         */
        void setHpanedTopChild() { setHPanedChild(0); }

        /** 
         * @brief Set a vpaned widget as top child.
         */
        void setVPanedTopChild() { setVPanedChild(0); }
        
        /** 
         * @brief Set a plugin as top child.
         */
        void setPluginTopChild() { setPluginChild(0); }

        /** 
         * @brief Set an empty widget as bottom child.
         */
        void setEmptyBottomChild() { setEmptyChild(1); }
        
        /** 
         * @brief Set a hpaned widget as bottom child.
         */
        void setHpanedBottomChild() { setHPanedChild(1); }
        
        /** 
         * @brief Set a vpaned widget as bottom child.
         */
        void setVPanedBottomChild() { setVPanedChild(1); }
        
        /** 
         * @brief Set a plugin as bottom child.
         */
        void setPluginBottomChild() { setPluginChild(1); }

        EOVchild& top() { return getChild(0); }
        
        const EOVchild& top() const { return getChild(0); }
        
        EOVchild& bottom() { return getChild(1); }
        
        const EOVchild& bottom() const { return getChild(1); }
    };



    class EOVplugin : public EOVchild
    {
    public:
        EOVplugin() 
        { }

        virtual ~EOVplugin()
        { }

        virtual EOVchild* clone() const
        { return new EOVplugin(*this); }

        void init(xmlpp::Element* elt);

        void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set information of a EOVplugin, ie, the output name and the
         * plugin attached to this output.
         * 
         * @param output name of the output.
         * @param plugin name of the plugin.
         */
        void setEOVplugin(const std::string& output,
                          const std::string& plugin);

        inline const std::string& output() const
        { return m_output; }

        inline const std::string& plugin() const
        { return m_plugin; }

    private:
        std::string m_output;
        std::string m_plugin;
    };



    /** 
     * @brief The base class of Eye of VLE program. It build a hierarchy of
     * plugin, vpaned or hpaned.
     */
    class EOV : public EOVcontainer
    {
    public:
        EOV() :
            EOVcontainer(1)
        { }

        virtual ~EOV()
        { }
        
        virtual EOVchild* clone() const
        { return new EOV(*this); }

        virtual void init(xmlpp::Element* elt);

        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set the EOV information, host using the syntax host:port.
         * 
         * @param host the host syntax.
         */
        void setEOV(const std::string& host);

        /** 
         * @brief Set current child as empty child.
         */
        void setEmptyChild() { EOVcontainer::setEmptyChild(0); }

        /** 
         * @brief Set current child as VPaned child.
         */
        void setVPanedChild() { EOVcontainer::setVPanedChild(0); }

        /** 
         * @brief Set current child as HPaned child.
         */
        void setHPanedChild() { EOVcontainer::setHPanedChild(0); }

        /** 
         * @brief Set current child as Plugin child.
         */
        void setPluginChild() { EOVcontainer::setPluginChild(0); }

        /** 
         * @brief Host where the eov run.
         * 
         * @return host using syntax host:port.
         */
        inline const std::string& host() const { return m_host; }

        /** 
         * @brief Set the host of this eov.
         * 
         * @param host using syntax host:port.
         */
        inline void host(const std::string& host) { m_host.assign(host); }

        /** 
         * @brief Get the child of EOV.
         * 
         * @return a reference to a child.
         */
        inline EOVchild& child() { return getChild(0); }

        /** 
         * @brief Get the child of EOV.
         * 
         * @return a reference to a child.
         */
        inline const EOVchild& child() const { return getChild(0); }

        std::string xml() const;

        std::list < const EOVplugin* > plugins() const;

    private:
        std::string     m_host;
    };

    
    
    /** 
     * @brief The base class of Eye of VLE program. It build a hierarchy of
     * plugin, vpaned or hpaned.
     */
    class EOVs : public Base
    {
    public:
        EOVs() { }

        virtual ~EOVs() { }

        virtual void init(xmlpp::Element* elt);
        
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Attach a new EOV to the EOVs list.
         * 
         * @param name name of the EOV.
         * @param e the EOV to cloned.
         */
        void addEOV(const std::string& name, const EOV& e);

        /** 
         * @brief Delete all vpz::EOV from list.
         */
        void clear();

        /** 
         * @brief Get the list of EOV.
         * 
         * @return list of EOV.
         */
        const std::map < std::string, EOV >& eovs() const
        { return m_lst; }

    private:
        std::map < std::string, EOV >   m_lst;
    };

}} // namespace vle vpz

#endif
