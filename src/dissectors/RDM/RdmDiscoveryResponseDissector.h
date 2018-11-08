class RdmDiscoveryResponseDissector : public PacketDissector
{
public:
    friend class RdmDissector;

    virtual quint8 startCode() const;
    virtual QVariant protocolName(const Packet &p, int role) const;
    virtual QVariant getSource(const Packet &p, int role) const;
    virtual QVariant getDestination(const Packet &p, int role) const;
    virtual QVariant getInfo(const Packet &p, int role) const;

	virtual int preprocessPacket(Packet &p, /*out*/ QList<Packet> &list) const;

private:
    static void extractRdmSourceDest(Packet &p);
};